#include "stdio.h"
#include "mkso.h"
#include "linkso.h"


int test_so(int x)
{

	linkso(x);

	printf("x:%d.\n",x);
	
	return x;
}
