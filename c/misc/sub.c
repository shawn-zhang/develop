#include <stdio.h>

#define DBL_DIG  15
#define DBL_MAX  1.7976931348623157E+308
#define DBL_MIN  2.2250738585072014E-308

#define FLT_DIG  6
#define FLT_MAX  3.40282347E+38F
#define FLT_MIN  1.17549435E-38F

int main(int argc, char *argv[])
{
    unsigned char x = 255;

    printf("x = %u\n", x);
    
    printf("DBL_MAX=%f\n", DBL_MAX);
    printf("DBL_MIN=%f\n", DBL_MIN);

    printf("FLT_MAX=%f\n", FLT_MAX);
    printf("FLT_MIN=%f\n", FLT_MIN);

    return 0;
}
