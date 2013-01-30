#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "framebuffer.h"
#include "bmpview.h"

struct __bmphead{
	unsigned short type;
	int size;
	unsigned short reserve1;
	unsigned short reserve2;
	long data_offset;
} __attribute__((packed));
struct __maphead{
	long size;
	long width;
	long height;
	short planes;
	short bit_count;
	long compress;
	long image_size;
	long xppm;
	long yppm;
	long clr_used;
	long clr_important; 
} __attribute__((packed));

static struct __fbdev fbdev;

int draw(const char *path,int st_x,int st_y)
{
	int bmpfd,ret,x,y;
	BMPHEAD bmphead;
	MAPHEAD maphead;

	strcpy(fbdev.dev,"/dev/fb0");
	if (fb_open(&fbdev) < 0){
		perror(fbdev.dev);
		goto returnerror;
	}
	bmpfd = open(path,O_RDONLY);
	if (bmpfd == -1){
		perror(path);
		goto returnerror1;
	}
	fb_clean(&fbdev);
	ret = read(bmpfd,&bmphead,sizeof(bmphead));
	if (ret < 0){
		perror("Read bmphead error");
		goto returnerror2;
	}
	ret = read(bmpfd,&maphead,sizeof(maphead));
	if (ret < 0){
		perror("Read maphead error");
		goto returnerror2;
	}
	x = st_x;
	y = st_y;
	if (st_x == -1){
		if ((x = (get_xres(&fbdev) - maphead.width)) < 0)
			x = 0;
		else
			x = x / 2;
	}
	if (st_y == -1){
		if ((y = (get_yres(&fbdev) - maphead.height)) < 0)
			y = 0;
		else
			y = y / 2;
		y = y + maphead.height - 1;
	}
	printf("maphead.width = %d,maphead.height = %d,screen_xres = %d,screen_y = %d\n",maphead.width,maphead.height,get_xres(&fbdev),get_yres(&fbdev));
	draw__(bmpfd, x, y,maphead.width,maphead.height);
	
	close(bmpfd);
	fb_close(&fbdev);
	
	return 0;


returnerror2:
	close(bmpfd);
returnerror1:
	fb_close(&fbdev);
returnerror:
	return -1;

}
void draw_alpha__(int fd_background,int fd_force,int x, int y, int width, int height)
{	
	
	unsigned char *buf = NULL, *buf_back = NULL, *buf_for = NULL;
	int i,buffsize,j;
	
	
	buffsize = (width * 3 + 4 - 1) / 4 * 4;
	buf_back = malloc(buffsize);
	if (buf_back == NULL){
		perror("malloc error");
	}
	buf_for = malloc(buffsize);
	if (buf_for == NULL){
		perror("malloc error");
	}
	buf = malloc(buffsize);
	if (buf == NULL){
		perror("malloc error");
	}


	for (i = 0; i < height; i++,y--){
		read(fd_background,buf_back,buffsize);
		read(fd_force,buf_for,buffsize);
		for(j = 0; j < buffsize; j++){
		    buf[j] = (int) (((float)(buf_back[j]) * 0.7) + ((float)(buf_for[j]) * 0.3));
		}
		draw_line(buf,buffsize,x,y);
	}
	free(buf_back);
	free(buf_for);
	free(buf);
}
int draw_alpha(const char *path1,const char *path2,int st_x,int st_y)
{	
	
	int bmpfd,bmp2fd,ret,x,y;
	BMPHEAD bmphead;
	MAPHEAD maphead;

	strcpy(fbdev.dev,"/dev/fb0");
	if (fb_open(&fbdev) < 0){
		perror(fbdev.dev);
		goto returnerror;
	}
	bmpfd = open(path1,O_RDONLY);
	if (bmpfd == -1){
		perror(path1);
		goto returnerror1;
	}
	bmp2fd = open(path2,O_RDONLY);
	if (bmp2fd == -1){
		perror(path2);
		goto returnerror1;
	}
	fb_clean(&fbdev);
	ret = read(bmpfd,&bmphead,sizeof(bmphead));
	if (ret < 0){
		perror("Read bmphead error");
		goto returnerror2;
	}
	ret = read(bmpfd,&maphead,sizeof(maphead));
	if (ret < 0){
		perror("Read maphead error");
		goto returnerror2;
	}
	ret = read(bmp2fd,&bmphead,sizeof(bmphead));
	if (ret < 0){
		perror("Read bmphead error");
		goto returnerror2;
	}
	ret = read(bmp2fd,&maphead,sizeof(maphead));
	if (ret < 0){
		perror("Read maphead error");
		goto returnerror2;
	}
	ret = lseek(bmp2fd,sizeof(bmphead) + sizeof(maphead),SEEK_SET);
	x = st_x;
	y = st_y;
	if (st_x == -1){
		if ((x = (get_xres(&fbdev) - maphead.width)) < 0)
			x = 0;
		else
			x = x / 2;
	}
	if (st_y == -1){
		if ((y = (get_yres(&fbdev) - maphead.height)) < 0)
			y = 0;
		else
			y = y / 2;
		y = y + maphead.height - 1;
	}
	printf("maphead.width = %d,maphead.height = %d,screen_xres = %d,screen_y = %d\n",maphead.width,maphead.height,get_xres(&fbdev),get_yres(&fbdev));
	draw_alpha__(bmpfd,bmp2fd, x, y,maphead.width,maphead.height);
	
	close(bmpfd);
	close(bmp2fd);
	fb_close(&fbdev);
	
	return 0;


returnerror2:
	close(bmpfd);
returnerror1:
	fb_close(&fbdev);
returnerror:
	return -1;

}


void draw__(int fd,int x,int y,int width,int height)
{
	unsigned char *buf;
	int i,buffsize;
	
	
	buffsize = (width * 3 + 4 - 1) / 4 * 4;
	buf = malloc(buffsize);

	for (i = 0; i < height; i++,y--){
		read(fd,buf,buffsize);
		draw_line(buf,width,x,y);
	}
	
	free(buf);
}


int draw_line(unsigned char *buf,int buffsize,int x,int y)
{
	int j;

	for (j = 0; j < buffsize; j++){
		draw_pixl_buf((PFBDEV)(&fbdev),buf + j * 3,3,x + j,y);
	}
	
	return 0;
}

