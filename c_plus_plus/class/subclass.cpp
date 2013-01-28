#include <stdio.h>

#include "subclass.h"

using namespace test;

subclass::subclass():base(1)
{
	printf("[test.subclass.subclass] constuct...\n");
}
subclass::subclass(int a):base(a)
{
	this->x = a;
	printf("suclass %d\n",this->x);
}
subclass::~subclass()
{

}

subclass::subclass(unsigned char *begin,unsigned char *end):begin_(begin),end_(end),x(3),base(1)
{
	t = 2;
	printf("[%s,%d]. \n",__FUNCTION__,__LINE__);
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
	this->x = argv1;
	printf("[test.subclass.func6] int argv %d\n",argv1);

}

void subclass::func6(unsigned char argv1)
{
	printf("[test.subclass.func6] unsigned char argv = %d\n",argv1);
}
