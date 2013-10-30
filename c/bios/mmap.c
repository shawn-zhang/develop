#include <stdio.h>
#include <sys/mman.h>


int main(int argc, char *argv[])
{

    void *mem = NULL;

    mem = mmap(0, (size_t)0x100000, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, -1, 0);

    if (mem != NULL){
        printf("error no 0 \n");
        return -1;
    }

    printf("OK it is 0 \n");

    return 0;
}
