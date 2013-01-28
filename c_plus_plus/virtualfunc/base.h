#ifndef _TEST_BASE_H
#define _TEST_BASE_H

namespace test
{
	class base
	{
		public:
			base();
			~base();
			virtual void func1(void);
			void func2(void);
			void func3(int a,int b = 1,int c = 2);
			//std::vector<int> v;
	};
	
}
#endif


