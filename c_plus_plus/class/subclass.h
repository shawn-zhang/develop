#ifndef _TEST_SUBCLASS_H
#define _TEST_SUBCLASS_H

#include "base.h"

namespace test{

	class subclass : public base
	{
		public:
			subclass();
			subclass(int a);
			subclass(unsigned char *begin,unsigned char *end);
			~subclass();
			void func1(void);
			subclass& operator<<(unsigned char d);
			void func6(int argv1);
			void func6(unsigned char argv1);
            int &func7(base&, int &a, int &b, int &c, int &d);
            int *func8(base *, int *a, int *b, int *c, int *d);
            subclass newInstance(void);

            int xxxx;

		private:
			unsigned char *begin_;
			unsigned char *end_;
			unsigned int x;
	};
}

#endif
