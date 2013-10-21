#include <stdio.h>


typedef struct tst_st{
    int x;
    int y;
}TST;


int main(int argc, char *argv[])
{
    TST tst, tmp_tst;

    tmp_tst.x = 5; 
    tmp_tst.y = 6;
    
    tst = tmp_tst;

    printf("x = %d, y = %d\n", tst.x, tst.y);

    return 0;
}
