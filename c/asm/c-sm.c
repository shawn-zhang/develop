#include <stdio.h>

int main(void)
{
    int tmp = 1;

    asm (
        "mov r0, #2 \r\n"
        "add %0, r0, #4, 20 \r\n"
        :"+r"(tmp) //output operator
        :    //input operator
        :"r0"
    );

    printf("tmp = %d\n", tmp);

    return 0;
}
