#include <stdio.h>


#define __init __attribute__ ((constructor))
#define __exit __attribute__ ((destructor))


static __init void constructor_test()
{

    printf("%s, %d\n", __func__, __LINE__);

}

static __exit void  destructor_test()
{
    printf("%s, %d\n", __func__, __LINE__);

}

int main(int argc, char *argv[])
{

    return 0;

}
