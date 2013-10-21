#include <stdio.h>

int main(int argc, char *argv[])
{
    
    unsigned char str[] = {"¿ªÊ¼"};
    int i = 0;

    for(i = 0; i < sizeof(str); i++){
        printf("0x%x\t", str[i]);
    }
    
    printf("\n");
    
    printf("%s\n", str);

    return 0;
}
