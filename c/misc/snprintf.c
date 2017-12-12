#include <stdio.h>

int main (int argc, char *argv[])
{
    int count = 0;
    char str[20] = {0}, *p = NULL;

    count = snprintf(str, 5, "\\x%02x", 0);
    p = str + count;
    *p = '\0';
    printf("i = %d, str = %s\n", count, str);
    
    *p++ = '5';
    *p = '\0';

    printf("str = %s\n", str);

    return 0;
}
