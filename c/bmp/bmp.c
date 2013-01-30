#include <stdio.h>
#include <stdlib.h>

#include <bmpview.h>
#include <framebuffer.h>


int main(int argc,char *argv[])
{
	int x = 0,y = 0;

	if (argc < 2){
		fprintf(stderr,"argument error\n");
		return -1;
	}
	if (argc > 3){
		x = atoi(argv[3]);
		if (argc > 3)
			y = atoi(argv[4]);
		else
			y = -1;
	}
	else
	{
		x = -1;
		y = -1;
	}
	draw_alpha(argv[1],argv[2],x,y);

	return 0;
}
