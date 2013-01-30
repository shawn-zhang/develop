#ifndef SH_FRAMBUF_H
#define SH_FRAMBUF_H

#include <linux/fb.h>


typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char uchar;
struct __fbdev{
	int fb;
	ulong fb_mem_offset;
	ulong fb_mem;
	struct fb_fix_screeninfo fb_fix;
	struct fb_var_screeninfo fb_var;
	char dev[20];
};
typedef struct __fbdev FBDEV;
typedef FBDEV *PFBDEV;

int fb_open(PFBDEV pfbdev);

void fb_close(PFBDEV pfbdev);

int get_display_depth(PFBDEV const pFbdev);

void fb_memset(void *addr, unsigned int c, size_t len); 

uint get_xres(PFBDEV const pfbdev);

uint get_yres(PFBDEV const pfbdev);

ulong get_offset(PFBDEV const pfbdev,int x,int y);

int draw_pixl(PFBDEV const pfbdev,int x,int y,unsigned int color);

int draw_pixl_buf(PFBDEV const pfbdev,uchar *buf_pixl,uint buffsize_pixl,int x,int y);

int is_fbdev(PFBDEV const pfbdev);

void fb_clean(PFBDEV pfbdev);

int is_cross_border(PFBDEV,int ,int );

#endif
