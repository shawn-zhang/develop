#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd, retval, n = 0;
    char buf[6];
    fd_set readfds;
    struct timeval tv;

    fd = open("/dev/input/mice", O_RDONLY);
    if(!fd)
    {
        printf("Failed to open \"/dev/input/mice\"\n");
        return -1;
    }

    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        if((retval = select(fd+1, &readfds, NULL, NULL, &tv)) == 1)
        {
            if((n = read(fd, buf, 6) )<= 0)
            {
                continue;
            }
            printf("n = %d, Button type = %d, X = %d, Y = %d\n", n, buf[0] & 0x07, buf[1], buf[2]);
        }
    }
    close(fd);
    return 0;
}
