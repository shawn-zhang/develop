#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/fb.h>
#include <sys/user.h> 

#include <stdio.h>
#include <string.h>

#include "framebuffer.h"

#define COLOR		0x0000ff



int fb_open(PFBDEV pfbdev)
{
	if (pfbdev->dev[0] == '\0')
		return -1;
	pfbdev->fb = open(pfbdev->dev,O_RDWR);
	if (pfbdev->fb == -1)
		return -1;
	if (-1 == ioctl(pfbdev->fb,FBIOGET_VSCREENINFO,&(pfbdev->fb_var)))
		goto returnfalse;
	if (-1 == ioctl(pfbdev->fb,FBIOGET_FSCREENINFO,&(pfbdev->fb_fix)))
		goto returnfalse;
	pfbdev->fb_mem_offset = (ulong)(pfbdev->fb_fix.smem_start) & (~PAGE_MASK);
	pfbdev->fb_mem = (unsigned long int)mmap(NULL, pfbdev->fb_fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, pfbdev->fb, 0); 

	if (-1L == pfbdev->fb_mem)
		goto returnfalse;
	
	
	return 0;

returnfalse:
	close(pfbdev->fb);
	pfbdev->fb = -1;
	return -1;
}

void fb_close(PFBDEV pfbdev)
{
	close(pfbdev->fb);
	munmap((void *)pfbdev->fb_mem,pfbdev->fb_fix.smem_len);
	pfbdev->fb = -1;
}

int get_display_depth(PFBDEV const pfbdev)	//if error :return -1;
{
	if (is_fbdev(pfbdev))
		return -1;
	return pfbdev->fb_var.bits_per_pixel;
}

void fb_memset(void *addr, unsigned int c, size_t len)
{
	memset(addr,c,len);
}

void fb_clean(PFBDEV pfbdev)
{
	memset((void *)(pfbdev->fb_mem),0,pfbdev->fb_fix.smem_len);
}

uint get_xres(PFBDEV const pfbdev)	//if error: return 0; else return var.res
{
	if(is_fbdev(pfbdev))
		return 0;
	else
		return pfbdev->fb_var.xres;
}

uint get_yres(PFBDEV const pfbdev)
{
	if (is_fbdev(pfbdev))
		return 0;
	else
		return pfbdev->fb_var.yres;
}

ulong get_offset(PFBDEV const pfbdev,int x,int y)
{
	if (is_fbdev(pfbdev))
		return 0;
	else
		return pfbdev->fb_mem + (get_display_depth(pfbdev) / 8) * (x + y * get_xres(pfbdev));
}

int draw_pixl(PFBDEV const pfbdev,int x,int y,unsigned int color)
{
	char *offset;	

	if (is_fbdev(pfbdev))
		return -1;
	offset = (char *)get_offset(pfbdev,x,y);
	*offset = (color & COLOR);
	*(offset + 1) = ((color >> 8) & COLOR);
	*(offset + 2) = ((color >> 16) & COLOR);

	return 0;
}

int draw_pixl_buf(PFBDEV const pfbdev,uchar *buf_pixl,uint buffsize_pixl,int x,int y)
{
	
	if (is_fbdev(pfbdev))
		return -1;
	if (is_cross_border(pfbdev,x,y))
		return -1;
	memcpy((uchar *)get_offset(pfbdev,x,y),buf_pixl,buffsize_pixl);

	return 0;
}

int is_fbdev(PFBDEV const pfbdev)
{
	if (pfbdev->fb < 0)
		return -1;
	else
		return 0;
}
int is_cross_border(PFBDEV const pfbdev,int x,int y)
{
	if (x > get_xres(pfbdev) || y > get_yres(pfbdev))
		return -1;
	else 
		return 0;
}
