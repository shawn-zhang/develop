#include "base.h"
#include <stdio.h>

using namespace test;

base::base()
{

}
base::~base()
{

}

void base::func1(void)
{
	printf("[test.base.func1] func1...\n");	
}

void base::func2(void)
{
	printf("[test.base.func2] func2...\n");
	func1();
}

void base::func3(int a, int b, int c)
{
	printf("[test.base.func3] a = %d,b = %d,c = %d\n",a,b,c);

}
