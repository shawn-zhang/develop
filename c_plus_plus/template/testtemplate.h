#ifndef SHAWN_TEST_TEMPLATE__
#define SHAWN_TEST_TEMPLATE__

using namespace std;
#include <iostream>

template <class T1,class T2>
class testtemplate
{
	private:
		T1 i;
		T2 j;
	public:
		testtemplate(T1 a,T2 b);
		void show();
};


template <class T1,class T2>
testtemplate<T1,T2>::testtemplate(T1 a,T2 b):i(a),j(b)
{

}
template <class T1,class T2>
void testtemplate<T1,T2>::show()
{
	cout << "I" << i << "j" << j << endl;
	//printf("i:%d j:%d .\n",i,j);
}


#endif
