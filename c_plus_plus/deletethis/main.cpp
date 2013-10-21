#include <stdio.h>
#include <iostream>

#include "LightRefBase.h"
#include "sp.h"

using namespace std;
using namespace test;

namespace test
{
    class tc : LightRefBase<tc>
    {
        public:
            tc(){}
            ~tc(){}
        private:
            int x;
    };
}


int main(int argc,char *argv[])
{
    sp<tc> base(new tc);


	return 0;
}
