#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xab465258, "module_layout" },
	{ 0xd0d8621b, "strlen" },
	{ 0x54e5f648, "sock_release" },
	{ 0xff1e79e9, "netlink_kernel_create" },
	{ 0x2bc95bd4, "memset" },
	{ 0x50eedeb8, "printk" },
	{ 0xb4390f9a, "mcount" },
	{ 0x641f8a91, "netlink_unicast" },
	{ 0xf1cd5cdd, "init_net" },
	{ 0x124a736d, "__alloc_skb" },
	{ 0x37cd5146, "kfree_skb" },
	{ 0x2e60bace, "memcpy" },
	{ 0x9e9d3f2b, "skb_put" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "7B7A6FB4BA3F863F93FB545");
