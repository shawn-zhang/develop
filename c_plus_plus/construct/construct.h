#ifndef __TEST_CONSTRUCT_H__
#define __TEST_CONSTRUCT_H__

namespace test{
class construct
{
	public:
		construct(){}
		construct(int a, int b):a_(a),b_(b)
		{
		
		}
		int result(){
			return a_ + b_;
		}
	private:
		int a_,b_;
};

}
#endif
