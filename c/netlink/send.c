
#include <stdio.h>
#include <socket.h>


int main(int argc, char *argv[])
{
    int sender_sock = -1;
    struct sockaddr_nl snl;

    sender_sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_GENERIC);
    if (sender_sock < 0){
        perror("socket");
        return -1
    }

    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;//这儿是发广播用的。如果是发往内核该值为 0；

    

}

