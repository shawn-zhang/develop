#include <stdio.h>


int main(int argc, char *argv[])
{
    unsigned int test = 1; 

    printf("is bigendian = %d \n",!*(unsigned char *)&test);

    return 0;
}
