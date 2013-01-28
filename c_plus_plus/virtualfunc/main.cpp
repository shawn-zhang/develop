#include <stdio.h>
#include "subclass.h"

using namespace test;

int main(int argc,char *argv[])
{
	unsigned char a[5];

	subclass sub(a,a+5);

	sub.func2();
	sub.func3(2);
	
	//sub.subclass(a,a+5);
	//sub.subclass();

	sub << 3;
	sub.func6(8);

	sub.func6(a[0]);


	return 0;
}

