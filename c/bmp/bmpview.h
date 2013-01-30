#ifndef SH_BMPVIEW_H
#define SH_BMPVIEW_H

struct __bmphead;
struct __maphead;

typedef struct __bmphead BMPHEAD;
typedef BMPHEAD *PBMPHEAD;
typedef struct __maphead MAPHEAD;
typedef MAPHEAD *PMAPHEAD;

int draw(const char *path,int st_x,int st_y);

int draw_line(unsigned char *buf,int buffsize,int x,int y);


void draw__(int fd,int x,int y,int width,int height);

#endif
