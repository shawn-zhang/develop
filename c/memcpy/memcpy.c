#include <stdio.h>

int main(int argc, char *argv[])
{
	char *str = "aaaa";
	int *p = str;

	printf("str = %d \n",(int *)str);
	
	printf("p = %d \n",*p);

	return 0;
}
