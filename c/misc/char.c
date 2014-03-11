#include <stdio.h>

static char a[4];
static int b;

int main(int argc, char *argv[])
{
    b = 200;
    
    memcpy(&b, a, 4);

    return 0;
}
