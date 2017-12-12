#include <stdio.h>

int main(int argc, char *argv[])
{
    off64_t alpha = -2, beta = 0;
    size_t  gamma = 1, delta = 0;

    if (alpha + gamma <= beta + delta) {
        printf("LINE: %d alpha + gamma = %ld\n", __LINE__, alpha + gamma);
    }

    beta = alpha + gamma;
    delta = alpha + gamma;

    printf("LINE: %d alpha + gamma = %ld\n", __LINE__, beta);
    printf("LINE: %d alpha + gamma = %lu\n", __LINE__, delta);

    return 0;

}
