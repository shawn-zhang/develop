#include <stdio.h>
#include <pthread.h>


static pthread_mutex_t sgMutex = PTHREAD_MUTEX_INITIALIZER;


static void *threadFunc(void *arg)
{
    printf("threadFunc enter\n");
    pthread_mutex_lock(&sgMutex);

    printf("lock\n");
    while(1);

    pthread_mutex_unlock(&sgMutex);

    return NULL;

}

int main(int argc, char *argv[])
{
    
    pthread_t thrid;
    
    printf("lock\n");
    
    pthread_create(&thrid, NULL, threadFunc, NULL);
    
    sleep(1);

    pthread_mutex_lock(&sgMutex);
    printf("unlock\n");

    pthread_mutex_unlock(&sgMutex);

    return 0;
}
