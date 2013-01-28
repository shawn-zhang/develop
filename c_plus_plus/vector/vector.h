#ifndef TEST_VECTOR_H
#define TEST_VECTOR_H


namespace test{
class TestClass{

	private:
		int type_;

	public:
		TestClass(int type):type_(type){}
		int getType(){
			return type_;
		}
};
}

#endif
