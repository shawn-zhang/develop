#include <stdio.h>
#include "vector.h"
#include <vector>

using namespace test;

int main(int argc, char *argv[])
{
	std::vector<TestClass> vtc(2,3);

	for(int i = 0; i < vtc.size(); i++){
		printf("%d\n",vtc.at(i).getType());
	}

	return 0;
}

