


#define USB_VENDOR_ID_1		0x174E
#define USB_VENDOR_ID_2		0x174F
#define USB_PRODUCT_ID_S	0x5360

struct usb_skel {
	struct usb_device *      udev;                 /* the usb device for this device */
	struct usb_interface *   interface;            /* the interface for this device */
	struct semaphore       limit_sem;         /* limiting the number of writes in progress */
	unsigned char *         bulk_in_buffer;     /* the buffer to receive data */
	size_t         bulk_in_size;                  /* the size of the receive buffer */
	__u8          bulk_in_endpointAddr;        /* the address of the bulk in endpoint */
	__u8          bulk_out_endpointAddr;      /* the address of the bulk out endpoint */
	struct kref   kref;
}; 


static struct usb_driver skel_driver = {
	.owner = THIS_MODULE,
	.name = "skeleton",
	.probe = skel_probe,
	.disconnect = skel_disconnect,
	.id_table = skel_table;
};

static ssize_t skel_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
	struct usb_skel *dev;
	int retval = 0;

	dev = (struct usb_skel *)file->private_data; //获取设备的信息

	/* do a blocking bulk read to get data from the device */
	retval = usb_bulk_msg(dev->udev, usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr), dev->bulk_in_buffer, min(dev->bulk_in_size, count), &count, HZ * 10);

	/* if the read was successful, copy the data to userspace */
	if (!retval) {
		if (copy_to_user(buffer, dev->bulk_in_buffer, count))    
			retval = -EFAULT;
		else
			retval = count;
	}

	return retval;
}

static ssize_t skel_write(struct file *file, const char *user_buffer, size_t count, loff_t *ppos)
{
    struct usb_skel *dev;
    int retval = 0;
    struct urb *urb = NULL;
    char *buf = NULL;
    size_t writesize = min(count, (size_t)MAX_TRANSFER);

    dev = (struct usb_skel *)file->private_data;

    /* verify that we actually have some data to write */
    if (count == 0)
        goto exit;

    /* limit the number of URBs in flight to stop a user from using up all RAM */
    if (down_interruptible(&dev->limit_sem)) {
        retval = -ERESTARTSYS;
        goto exit;
    }

    spin_lock_irq(&dev->err_lock);
    if ((retval = dev->errors) < 0) {
        /* any error is reported once */
        dev->errors = 0;
        /* to preserve notifications about reset */
        retval = (retval == -EPIPE) ? retval : -EIO;
    }
    spin_unlock_irq(&dev->err_lock);
    if (retval < 0)
        goto error;

    /* create a urb, and a buffer for it, and copy the data to the urb */
    /*当驱动有数据发送到 USB 设备，首先分配一个 urb */
    urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!urb) {
        retval = -ENOMEM;
        goto error;
    }

    /*以最有效的方式是创建一个 DMA 缓冲区来发送数据到设备, 并拷贝数据到缓冲区*/
    buf = usb_buffer_alloc(dev->udev, writesize, GFP_KERNEL, &urb->transfer_dma);
    if (!buf) {
        retval = -ENOMEM;
        goto error;
    }

    if (copy_from_user(buf, user_buffer, writesize)) {
        retval = -EFAULT;
        goto error;
    }

    /* this lock makes sure we don't submit URBs to gone devices */
    mutex_lock(&dev->io_mutex);
    if (!dev->interface) {        /* disconnect() was called */
        mutex_unlock(&dev->io_mutex);
        retval = -ENODEV;
        goto error;
    }

    /* initialize the urb properly */
    /*在将urb提交给 USB 核心之前，正确初始化 urb */
    usb_fill_bulk_urb(urb, dev->udev,
             usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
             buf, writesize, skel_write_bulk_callback, dev);
    urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
    usb_anchor_urb(urb, &dev->submitted);

    /* send the data out the bulk port */
    /*提交 urb 给 USB 核心, 由它将 urb 传递给设备*/
    retval = usb_submit_urb(urb, GFP_KERNEL);
    mutex_unlock(&dev->io_mutex);
    if (retval) {
        err("%s - failed submitting write urb, error %d", __func__, retval);
        goto error_unanchor;
    }

    /* release our reference to this urb, the USB core will eventually free it entirely */
    usb_free_urb(urb);


    return writesize;

error_unanchor:
    usb_unanchor_urb(urb);
error:
    if (urb) {
        usb_buffer_free(dev->udev, writesize, buf, urb->transfer_dma);
        usb_free_urb(urb);
    }
    up(&dev->limit_sem);

exit:
    return retval;
}

//当urb被成功传递到 USB 设备(或者在传输中发生了错误), urb 回调函数将被 USB 核心调用.也就是上面初始化 urb 中的 skel_write_bulk_callback
static void skel_write_bulk_callback(struct urb *urb)
{
    struct usb_skel *dev;

    dev = urb->context;

    /* sync/async unlink faults aren't errors */
    /*检查 urb 的状态，判断这个 urb 是否成功完成传输*/
    if (urb->status) {
        if(!(urb->status == -ENOENT ||
         urb->status == -ECONNRESET ||
         urb->status == -ESHUTDOWN))
            err("%s - nonzero write bulk status received: %d",
             __func__, urb->status);

        spin_lock(&dev->err_lock);
        dev->errors = urb->status;
        spin_unlock(&dev->err_lock);
    }

    /* free up our allocated buffer */
    /*释放分配给这个 urb 的缓冲区.*/
    usb_buffer_free(urb->dev, urb->transfer_buffer_length,
            urb->transfer_buffer, urb->transfer_dma);
    up(&dev->limit_sem);
}



static int skel_open(struct inode *inode, struct file *file)
{
	struct usb_skel *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode); //获取设备的次设备号

	interface = usb_find_interface(&skel_driver, subminor);
	if (!interface) {
		err ("%s - error, can't find device for minor %d",
				__FUNCTION__, subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface); //获取注册到接口的usb??????skel数据。
	if (!dev) {
		retval = -ENODEV;
		goto exit;
	}

	/* increment our usage count for the device */
	kref_get(&dev->kref); //对usb_skel模块的使用计数

	/* save our object in the file's private structure */
	file->private_data = dev; //保存usb_skel结构的数据以便read，write函数使用

exit:
	return retval;
}

static struct file_operations skel_fops = {
	.owner =    THIS_MODULE,
	.read =     skel_read,
	.write =    skel_write,
	.open =     skel_open,
	.release =  skel_release,
};


#define USB_SKEL_MINOR_BASE          192                                     // minor base
static struct usb_class_driver skel_class = {
	.name = "usb/skel%d",
	.fops = &skel_fops,
	.minor_base = USB_SKEL_MINOR_BASE,
};


static int usb_probe( struct usb_interface *intf, const struct usb_device_id *id )
{
	struct usb_skel *skel = NULL;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
	int i;
	int retval = -ENOMEM;


	skel = kzalloc(sizeof(struct usb_skel), GFP_ATOMIC);
	if (skel == NULL){
		printk("[%s:%d] Error \n",__func__,__LINE__);
		return -ENOMEM;
	}

	skel->udev = usb_get_dev( interface_to_usbdev( intf ) );
	skel->interface = intf;
	kref_init( &skel->kref );

	/* set up the endpoint information */
	/* use only the first bulk-in and bulk-out endpoints */
	iface_desc = intf->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i){

		endpoint = &iface_desc->endpoint[i].desc;
		if (!skel->bulk_in_endpointAddr && (endpoint->bEndpointAddress & USB_DIR_IN) && ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK)) { 
			/* we found a bulk in endpoint */ 
			buffer_size = endpoint->wMaxPacketSize;
			skel->bulk_in_size = buffer_size;
			skel->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			skel->bulk_in_buffer = kzalloc(buffer_size, GFP_KERNEL);
			if (!dev->bulk_in_buffer) {
				err("Could not allocate bulk_in_buffer");
				goto error;
			}
		}

		if (!dev->bulk_out_endpointAddr && !(endpoint->bEndpointAddress & USB_DIR_IN) && ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK)) { 
			/* we found a bulk out endpoint */ 
			skel->bulk_out_endpointAddr = endpoint->bEndpointAddress;
		}
	}
	if (!(skel->bulk_in_endpointAddr && skel->bulk_out_endpointAddr))
	{

		err("Could not find both bulk-in and bulk-out endpoints");
		goto error;
	}

	/* save our data pointer in this interface device */
	usb_set_intfdata(intf, skel);

	/* we can register the device now, as it is ready */
	retval = usb_register_dev(intf, &skel_class);
	if (retval) {
		/* something prevented us from registering this driver */
		err("Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	/* let the user know what node this device is now attached to */
	info("USB Skeleton device now attached to USBSkel-%d", interface->minor);
	return 0;

error:
	if (dev)
		kref_put(&dev->kref, skel_delete);
	return retval;

}

static void usb_disconnect( struct usb_interface *intf )
{
	struct usb_skel *dev;
	int minor = interface->minor;

	/* prevent skel_open() from racing skel_disconnect() */
	lock_kernel();

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	/* give back our minor */
	usb_deregister_dev(interface, &skel_class);

	unlock_kernel();

	/* decrement our usage count */
	kref_put(&dev->kref, skel_delete);

	info("USB Skeleton #%d now disconnected", minor);
}

static struct usb_device_id skel_table[] = {
	{USB_DEVICE(USB_VENDOR_ID_1, USB_PRODUCT_ID_S)},
	{USB_DEVICE{USB_VENDOR_ID_2, USB_PRODUCT_ID_S}},
	{ }
};

MODULE_DEVICE_TABLE(usb, skel_table);

static int __init usb_skel_init(void)
{
	int result;
	
	result = usb_register(&skel_driver);
	if (result)
		err("usb_register failed. Error number %d", result);
	return result;
}
static void __exit usb_skel_exit(void)
{
	usb_deregister(&skel_driver);
}
module_init (usb_skel_init);
module_exit (usb_skel_exit);
MODULE_AUTHOR("Shawn Zhang");
MODULE_DESCRIPTION("USB Test Driver");
MODULE_LICENSE("GPL");
