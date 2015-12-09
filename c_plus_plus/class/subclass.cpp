#include <stdio.h>

#include "subclass.h"

#include <stdlib.h>

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

int &subclass::func7(base &bs, int &a, int &b, int &c, int &d)
{
    bs.func1();
    int *ret = (int *)malloc(sizeof(int));
    *ret = a + b + c + d;
    return *ret;
}

int *subclass::func8(base *bs, int *a, int *b, int *c, int *d)
{
    bs->func1();
    int *ret2 = (int *)malloc(sizeof(int));
    *ret2 = *a + *b + *c + *d;
    return ret2;
}

subclass subclass::newInstance(void)
{
    subclass sc;
    return sc;
}
