#include <stdio.h>


int main(int argc, char *argv[])
{
    unsigned int test = 1; 

#ifdef __MIPSEB__
#error __ARMEB__ define
#elif (defined __MIPSEL__)
#error __ARMEB__ notdefine
#endif
    printf("is bigendian = %d \n",!*(unsigned char *)&test);

    return 0;
}
