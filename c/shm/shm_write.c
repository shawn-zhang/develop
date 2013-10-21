#include <stdio.h>

#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[])
{
    key_t g_iShmKey;
    int g_iShareMemID = -1;
    int semid = -1;

    g_iShmKey = ftok(".",'w');
    if (g_iShmKey == -1){
    
        perror("ftok error");
        return  -1;
    }
    printf("g_iShmKey = 0x%x\n", g_iShmKey);

    if ((g_iShareMemID = shmget(g_iShmKey, 1024, IPC_CREAT|IPC_EXCL|0666)) == -1){
        perror("shmget error");
        return -1;
    }
    semid = semget(g_iShmKey, 1, IPC_CREAT| IPC_EXCL | 0666);
    if (semid == -1){
        perror("shmget error");
        return -1;
    }
    printf("g_iShmKey = 0x%x\n", g_iShmKey);

    return 0;
}
