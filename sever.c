#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SERV_PORT "6666"
#define SERV_IP_ADDR "192.168.41.3"
#define QUIT "quit"

#define BACKLOG 10

int StartUp(int fd);

int StartUp(int fd)
{
    //创建套接字
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        exit(1);
    }
    puts("---创建套接字成功----");

    //套接字多路复用
    int rb_use = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &rb_use, sizeof(int)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    //创建套接字结构体
    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(SERV_PORT));
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    puts("----创建结构体成功----");

    //绑定服务器
    if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("bind");
        exit(1);
    }
    puts("----绑定服务器成功----");

    //开启监听
    if (listen(fd, BACKLOG) < 0)
    {
        perror("listen");
        exit(1);
    }

    return fd;
}

int main(int argc, char const *argv[])
{
    int fd = -1;
    fd = StartUp(fd); //开启网络函数

    //关心是谁连接，创建结构体存放连接结构体的信息
    struct sockaddr_in cin;
    bzero(&cin, sizeof(cin));
    socklen_t addrlen = sizeof(cin);
    int newfd = accept(fd, (struct sockaddr *)&cin, &addrlen);
    if (newfd < 0)
    {
        perror("accept:");
        exit(1);
    }
    puts("----连接成功----");

    //打印建立连接的客户端
    char ipv4_addr[16] = {};
    if (!inet_ntop(AF_INET, (void *)&cin.sin_addr.s_addr, ipv4_addr, sizeof(cin)))
    {
        perror("inet_ntop:");
        exit(1);
    }
    printf("+++客户端%s:%d已连接+++\n", ipv4_addr, ntohs(cin.sin_port));

    //读写
    int ret = -1;
    char buf[BUFSIZ] = {};
    while (1)
    {
        bzero(buf, BUFSIZ);
        do
        {
            ret = recv(newfd, buf, BUFSIZ - 1, 0);
        } while (ret < 0 && errno == EINTR);
        if (ret < 0)
        {
            perror("recv:");
            exit(1);
        }
        if (!ret)
            break;

        printf("来自客户端：%s\n", buf);

        if (strncasecmp(QUIT, buf, strlen(QUIT)) == 0)
        {
            puts("----客户端已退出----");
            break;
        }
    }

    shutdown(fd, SHUT_RDWR);
    shutdown(newfd, 2);

    return 0;
}
