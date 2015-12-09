#include <stdio.h>
#include "subclass.h"

using namespace test;

static void callref(subclass &sc)
{
    int a, b = 6, c = 7, d = 8;
    
    sc.xxxx = 5;
    a = sc.func7(sc, sc.xxxx, b, c, d);
    printf("%d\n", a);
}

static void callptr(subclass *sc)
{
    int *a, b = 6, c = 7, d = 8; 
    sc->xxxx = 5;
    a = sc->func8(sc, &(sc->xxxx), &b, &c, &d);
    printf("%d\n", *a);
}

int main(int argc,char *argv[])
{
	//unsigned char a[5];

	//subclass sub(a,a+5);

	//sub.func2();
	//sub.func3(int(2));
	
	//sub.subclass(a,a+5);
	//sub.subclass();

	//sub << 3;
	//sub.func6(8);

	//sub.func6(a[0]);
    
    subclass sc;

    callptr(&sc);
    callref(sc);
    
    subclass scc = sc.newInstance();


	return 0;
}

