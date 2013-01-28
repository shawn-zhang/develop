#include <stdio.h>

#include "subclass.h"

using namespace test;

subclass::subclass()
{
	printf("[test.subclass.subclass] constuct...\n");
}
subclass::~subclass()
{

}

subclass::subclass(unsigned char *begin,unsigned char *end):begin_(begin),end_(end)
{
	subclass();
}

void subclass::func1(void)
{

	printf("[test.subclass.func1] func1....\n");
}


subclass& subclass::operator<<(unsigned char d)
{
	printf("[test.subclass.<<] d = %d\n",d);

	if (begin_ > end_){
		return *this;
	}
	
	*begin_++ = d;

	return *this;
}

void subclass::func6(int argv1)
{
	printf("[test.subclass.func6] int argv %d\n",argv1);

}

void subclass::func6(unsigned char argv1)
{
	printf("[test.subclass.func6] unsigned char argv = %d\n",argv1);
}
