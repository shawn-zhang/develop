
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>

//#define NETLINK_TEST 31 // 自定义的协议号  

#define NETLINK_TEST  17

/** 消息类型 **/
#define NLMSG_GETECHO  (0x10 + 2)
/** 最大协议负荷(固定) **/
#define MAX_PAYLOAD 101

struct sockaddr_nl src_addr, dst_addr;
struct iovec iov;
int sockfd;
struct nlmsghdr *nlh = NULL;
struct msghdr msg;

int main( int argc, char *argv[])
{
        if (argc != 2) {
                printf("usage: ./a.out <str>\n");
                exit(-1);
        }
        sockfd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_TEST); // 创建NETLINK_TEST协议的socket
        /* 设置本地端点并绑定，用于侦听 */
        bzero(&src_addr, sizeof(src_addr));
        src_addr.nl_family = AF_NETLINK;
        src_addr.nl_pid = getpid();
        src_addr.nl_groups = 0; //未加入多播组
        bind(sockfd, (struct sockaddr*)&src_addr, sizeof(src_addr));
        /* 构造目的端点，用于发送 */
        bzero(&dst_addr, sizeof(dst_addr));
        dst_addr.nl_family = AF_NETLINK;
        dst_addr.nl_pid = 0; // 表示内核
        dst_addr.nl_groups = 0; //未指定接收多播组
        /* 构造发送消息 */
        nlh = malloc(NLMSG_SPACE(MAX_PAYLOAD));
        nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD); //保证对齐
        nlh->nlmsg_pid = getpid();  /* self pid */
        nlh->nlmsg_flags = 0;
        nlh->nlmsg_type = NLMSG_GETECHO;
        strcpy(NLMSG_DATA(nlh), argv[1]);
        iov.iov_base = (void *)nlh;
        iov.iov_len = nlh->nlmsg_len;
        msg.msg_name = (void *)&dst_addr;
        msg.msg_namelen = sizeof(dst_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        sendmsg(sockfd, &msg, 0); // 发送
        /* 接收消息并打印 */
        memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
        recvmsg(sockfd, &msg, 0);
        printf(" Received message payload: %s\n", (char *)NLMSG_DATA(nlh));
        return 0;
}
