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

#define  SERV_PORT    "6666"
#define  SERV_IP_ADDR "192.168.1.106"
#define  QUIT         "quit"

#define BACKLOG       10

int StartUp(int fd);

int StartUp(int fd)
{
    //创建套接字
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("socket");
        exit(1);
    }

    //套接字多路复用
    int rb_use = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &rb_use, sizeof(int)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    //创建套接字结构体
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(SERV_PORT));
    sin.sin_addr.s_addr = htonl(SERV_IP_ADDR);

    //绑定服务器
    if(bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("bind");
        exit(1);
    }    

    //开启监听
    if(listen(fd, BACKLOG) < 0)
    {
        perror("listen");
        exit(1);
    }

    return fd;
}


int main(int argc, char const *argv[])
{
    int fd = -1;
    fd = StartUp(fd);   //开启网络函数
    
       
    return 0;
}
