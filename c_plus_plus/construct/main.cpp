#include <stdio.h>

#include "construct.h"

using namespace test;

construct c;

int main(int argc,char *argv[])
{
	c.construct(2,3);
	printf("add = %d\n",c.result());

	return 0;
}
