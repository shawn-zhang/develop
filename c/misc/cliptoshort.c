#include <stdio.h>




static __inline short CLIPTOSHORT(int x)
{
    int sign;

    /* clip to [-32768, 32767] */
    sign = x >> 31;
    if (sign != (x >> 15))
    {
        x = sign ^ ((1 << 15) - 1);
    }

    return (short)x;
}


int main(int argc, char *argv[])
{

    int x = 4000;
    int y = 65535;
    int z = 32769;

    printf("4000 = %d, 65535 = %d, 32769 = %d \n", CLIPTOSHORT(x), CLIPTOSHORT(y), CLIPTOSHORT(z));

    return 0;
}
