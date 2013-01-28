#include <stdio.h>
#include "vector.h"
#include <vector>

using namespace test;

int main(int argc, char *argv[])
{
	TestClass tc(3);
	std::vector<TestClass> vtc(2,tc);

	for(int i = 0; i < vtc.size(); i++){
		printf("%d\n",vtc.at(i).getType());
	}

	return 0;
}

