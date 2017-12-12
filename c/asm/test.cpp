#include <test.h>

S GetS(int value)
{
    S s;
    s.Value1 = value;
    s.Value2 = value + 2;
    s.Value3 = value + 3;
    
    return s;
}
