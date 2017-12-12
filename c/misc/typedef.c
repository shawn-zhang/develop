#include <stdio.h>


typedef unsigned int test48[48];
typedef test48 typedefine;


int main(int argc, char *argv[])
{

    printf("typedefine size = %lu, test48 = %lu\n", sizeof(typedefine), sizeof(test48));

    return 0;
}
