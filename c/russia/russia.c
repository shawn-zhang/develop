#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <string.h>

#include "framebuffer.h"


#define BLOCK_WIDTH 	12
#define FRAME_WIDTH	2
#define FRAME_COLOR	0x208080
#define WIDTH   12
#define HIGHT   22
#define X_POS   400
#define Y_POS   300
#define N_X_POS (X_POS + 100 + (WIDTH - 1) * BLOCK_WIDTH)
#define N_Y_POS Y_POS
#define N_WIDTH 8
#define N_HIGHT 6
#define COMP_SCORE ((n * 10)  + (n * n * 10))



struct block{
	int x[4],y[4];
	unsigned int color;
	int next;
};

FBDEV fbdev;
unsigned int board[WIDTH][HIGHT],next_board[8][6];
int slave_x,slave_y,x,y,score,n;
unsigned int color;
int a_n = 1,level = 1;
struct block nowblock,__change_block,next_block;
struct block shape[19] = {{0,1,2,3,0,0,0,0,0xFF0000,1},\
	{0,0,0,0,0,1,2,3,0x004000,0},\
	{0,0,0,1,0,1,2,2,0x004600,3},\
	{0,0,1,2,0,1,0,0,0x005000,4},\
	{0,1,1,1,0,0,1,2,0x800000,5},\
	{2,2,1,0,0,1,1,1,0x8000a0,2},\
	{0,0,1,1,0,1,0,1,0x804040,6},\
	{0,1,1,1,2,0,1,2,0xa0ffff,8},\
	{0,0,1,2,0,1,1,1,0xc040ff,9},\
	{0,0,0,1,0,1,2,0,0xff0000,10},\
	{0,1,2,2,0,0,0,1,0xff00c0,7},\
	{0,1,1,2,0,0,1,1,0xffff00,12},\
	{0,0,1,1,1,2,0,1,0xff8060,11},\
	{0,1,1,2,1,0,1,0,0xc0ff00,14},\
	{0,0,1,1,0,1,1,2,0x2040ff,13},\
	{1,0,1,2,0,1,1,1,0x00a0ff,16},\
	{0,0,1,0,0,1,1,2,0x408000,17},\
	{0,1,2,1,0,0,0,1,0x40ff00,18},\
	{1,0,1,1,0,1,1,2,0x60a040,15}};
void b_create()
{
	nowblock = next_block;
	next_block = shape[rand() % 19];
	x = 4;

}


void regect(int x1,int x2,int y1,int y2,unsigned int color)
{ 
  int i,j;
  for (i = x1; i < x2; i++)
    for (j = y1; j < y2; j++)    
       draw_pixl(&fbdev,i, j, color);
}


void print_score()
{
	printf("\33[26;76H");
	printf("Score : %05d",score);
	printf("\33[27;76H");
	printf("Level : %03d",level);
}

void board_init()
{

	int i,j;
	for(i = 1;i < WIDTH - 1; i++)
		for(j =1; j < HIGHT - 1; j++)
			board[i][j] = 0;

	for (i = 0; i < HIGHT; i ++){
		board[i][0] = 2;
		board[i][21] = 2;
	}
	for (i = 0; i < HIGHT; i++ ){
		board[0][i] = 2;
		board[11][i] = 2;
	}
	for(i = 1;i < N_HIGHT - 1; i++)
		for(j = 0; j < N_WIDTH; j++)
			next_board[i][j] = 0;
	for (i = 0;i < N_WIDTH; i++){
		next_board[i][0] = 2;
		next_board[i][N_HIGHT - 1] = 2;

	}

}

void frame_flush()
{
	int frame_w,frame_h;	

	frame_w = WIDTH - 1;
	frame_h = HIGHT - 1;
	
	regect(	X_POS + BLOCK_WIDTH - FRAME_WIDTH,\
		X_POS + frame_w * BLOCK_WIDTH,
		Y_POS + BLOCK_WIDTH - FRAME_WIDTH,\
		Y_POS + BLOCK_WIDTH, \
		FRAME_COLOR);
	regect(	X_POS + BLOCK_WIDTH - FRAME_WIDTH,\
		X_POS + frame_w * BLOCK_WIDTH,\
		Y_POS + frame_h * BLOCK_WIDTH,\
		Y_POS + frame_h * BLOCK_WIDTH + FRAME_WIDTH,\
		FRAME_COLOR);

	regect(	X_POS + BLOCK_WIDTH - FRAME_WIDTH,\
		X_POS + BLOCK_WIDTH,\
		Y_POS + BLOCK_WIDTH - FRAME_WIDTH,\
		Y_POS + frame_h * BLOCK_WIDTH, \
		FRAME_COLOR);
	regect(	X_POS + frame_w * BLOCK_WIDTH,\
		X_POS + frame_w * BLOCK_WIDTH + FRAME_WIDTH,\
		Y_POS + BLOCK_WIDTH - FRAME_WIDTH,\
		Y_POS + frame_h * BLOCK_WIDTH,\
		FRAME_COLOR);

}



void board_flush()
{
	
	int i,j;
	for(i = 1; i < WIDTH - 1; i++)
		for(j = 1; j < HIGHT - 1; j++){
			if(board[i][j] == 0)
				color = 0x000000;
			else if(board[i][j] == 1)
				color = nowblock.color;
			else if(board[i][j] == 2)
				color = FRAME_COLOR;
			else 
				color = board[i][j];
			regect(	i * BLOCK_WIDTH + X_POS,\
				i * BLOCK_WIDTH + BLOCK_WIDTH + X_POS,\
				j * BLOCK_WIDTH + Y_POS,\
				j * BLOCK_WIDTH + BLOCK_WIDTH + Y_POS,\
				color);
		}

}

void next_board_flush()
{
	int i,j;
	for(i = 0; i < N_WIDTH; i++)
		for(j = 0; j < N_HIGHT; j++){
			if(next_board[i][j] == 0)
				color = 0x000000;
			if(next_board[i][j] == 1){
				color = next_block.color;
			}
			if(next_board[i][j] == 2)
				color = 0x000000;
			regect(i * 10 + N_X_POS ,i * 10 + 10 + N_X_POS,j * 10 + N_Y_POS ,j * 10 + 10 + N_Y_POS,color);
		}

}

void clear_next_block()
{
	
	int i;
	for (i = 0; i < 4; i ++){
		slave_x = next_block.x[i] + 2;
		slave_y = next_block.y[i] + 1;
		if (next_board[slave_x][slave_y] == 1)
			next_board[slave_x][slave_y] = 0;
	
	}

}

void clear_block()
{
	int i;
	for (i = 0; i < 4; i ++){
		slave_x = nowblock.x[i] + x;
		slave_y = nowblock.y[i] + y;
		if (board[slave_x][slave_y] == 1)
			board[slave_x][slave_y] = 0;
	
	}

}
void drawboard()
{	
	
	int i;
//	clear_block();
	//y = y + 1;
	for(i = 0; i < 4; i++){
		slave_x = nowblock.x[i] + x; 
		slave_y = nowblock.y[i] + y;
		if (board[slave_x][slave_y] == 0)
			board[slave_x][slave_y] = 1;
		else
			break;
	}
}


void draw_block_stop()
{	
	
	int i;
	for(i = 0; i < 4; i++){
		slave_x = nowblock.x[i] + x; 
		slave_y = nowblock.y[i] + y;
		if (board[slave_x][slave_y] == 0)
			board[slave_x][slave_y] = nowblock.color;
		else
			break;
	}
}

void draw_next_board()
{
	
	int i;
	for (i = 0; i < 4; i ++){
		slave_x = next_block.x[i] + 2;
		slave_y = next_block.y[i] + 1;
		if (next_board[slave_x][slave_y] == 0)
			next_board[slave_x][slave_y] = 1;
	
	}
}

void check_sta()
{
	int i,j,m,flag = 0,flag1 = 0;
	n = 0;
	for (i = HIGHT - 2; i > 0; i--){
		for(j = 1; j < WIDTH - 1; j++){
			if (board[j][i] == 0){
				flag = 1;
				break;
			}
		}
		if (flag == 0){
			n++;
			for (m = i; m > 1; m--){
				for (j = 1; j < WIDTH - 1; j++){
					if (board[j][m-1] > 2)
						flag1 = 1;
					board[j][m] = board[j][m-1];
				}
				if (flag1 == 0)
					break;
			}
			i++;
		}
		flag = 0;
	}
	score = score + COMP_SCORE;
	if (score == 10000  / level ){
		score = 0;
		level++;
	}
	print_score();
	n = 0;

}
int check_board_stop()
{
	int i;
	for (i = 0; i < 4; i ++){
		slave_x = nowblock.x[i] + x;
		slave_y = nowblock.y[i] + y + 1;
		if( board[slave_x][slave_y] != 0)
			return -1;
	
	}
	return 0;

}

void while_pro()
{

	drawboard();
	board_flush();
	clear_block();

}

void move_down()
{	
	if (check_board_stop() == -1){
		//drawboard();
		draw_block_stop();
		check_sta();
		y = 1;
		clear_next_block();
		b_create();
		draw_next_board();
		next_board_flush();
		return ;
	}
	y++;

}

int move_left()
{
	int i;	
	for (i = 0; i < 4; i ++){
		slave_x = nowblock.x[i] + x - 1;
		slave_y = nowblock.y[i] + y;
		if( board[slave_x][slave_y] != 0)
			return -1;
	
	}
	clear_block();
	x--;
	while_pro();
	return 0;
}
int move_right()
{
	
	int i;	
	for (i = 0; i < 4; i ++){
		slave_x = nowblock.x[i] + x + 1;
		slave_y = nowblock.y[i] + y;
		if( board[slave_x][slave_y] != 0)
			return -1;
	
	}
	clear_block();
	x++;
	while_pro();
	return 0;

}
void change_block()
{
	int i;
	__change_block = shape[nowblock.next];
	for (i = 0; i < 4; i ++){
		slave_x = __change_block.x[i] + x;
		slave_y = __change_block.y[i] + y;
		if( board[slave_x][slave_y] != 0)
			return;
	}
	__change_block.color = nowblock.color;
	nowblock = __change_block;
}



void handler(int signo)
{
	int i;

	if (signo == SIGALRM){
		for (i = 0; i < level; i++) {
			clear_block();
			move_down();
			while_pro();
			check_sta();
		}
		alarm(1);
	}
}


void init()
{
	strcpy(fbdev.dev,"/dev/fb0");

	signal(SIGALRM,handler);
	alarm(1);
	slave_y= 0,x = 4,y = 1;
	next_block = shape[rand() % 19];
}


int main(void)
{
	int ret,pause = 0,flag = 0;
      	struct termios old, new;
      	char c;
	
	srand(getpid());
	init();
	ret = fb_open(&fbdev);
	if (ret != 0) {
		fprintf(stderr, "fb_open() error.\n");
	  	exit(1);
     	}

	tcgetattr(0,&new);
      	tcgetattr(0,&old);
     	new.c_lflag = new.c_lflag & ~(ICANON | ECHO);
	new.c_cc[VMIN] = 1;
	new.c_cc[VTIME] = 0;
	
	tcsetattr(0, TCSANOW, &new);
	board_init();
	frame_flush();
	b_create();
	clear_next_block();
	draw_next_board();
	next_board_flush();
	print_score();
	while(1){
		c = getchar();
		switch (c){
			case 'a':move_left();break;
			case 'd':move_right();break;
			case 's':move_down();while_pro();break;
			case 'w':change_block();while_pro();break;
			case ' ':
				alarm((++pause) % 2); 
				if (pause > 10) 
					pause = 0;
				break; 
			case 'q':flag = 1;printf("\n");break;

			default:
				break;
		}
		if (flag == 1)
			break;
	 	fflush(NULL);
	 }
      	 tcsetattr(0, TCSANOW, &old);
	 fb_close(&fbdev);
	 return 0;
}
