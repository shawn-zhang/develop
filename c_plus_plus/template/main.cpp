#include "testtemplate.h"
#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc,char *argv[])
{

	testtemplate<int,int> testtemplate1(2,3);
	//testtemplate testtemplate1(2,3);

	testtemplate1.show();

	return 0;
}
