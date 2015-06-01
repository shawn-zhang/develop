#include <stdio.h>


int main(int argc, char *argv[])
{
    int i = 3;
    int k = (++i) + (++i) + (++i);


    printf("test.k = %d\n", k);

    return 0;
}
