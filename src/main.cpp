#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <error.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Logger.h"

#define NO 0
#define WRITE 1
#define READ 4
#define CONNECT 2
#define CLOSE 3

using namespace std;

int main()
{
    ////////////////////////////////////////
    /*           epoll variables          */
    int epoll_fd = epoll_create(10);
    struct epoll_event event, *events = NULL;
    event.events = EPOLLIN;
    ////////////////////////////////////////
    ////////////////////////////////////////
    /*        connection setup            */
    int sockfd = -1;
    if(0 > (sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
        slogf(ERROR, "socket");
    }

    struct sockaddr_in s_addr;
    bzero((char*)&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(10000);

    if(0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&s_addr, sizeof(s_addr)))
        slogf(ERROR, "%s", strerror(errno));

    if(0 > bind(sockfd, (struct sockaddr*)&s_addr, sizeof(s_addr))) {
        slogf(ERROR, "bind");
    }

    if(0 > listen(sockfd, 3)) {
        slogf(ERROR, "listen");
    }

    int connfd = -1;
    struct sockaddr_in c_addr;
    socklen_t len = sizeof(c_addr);
    if(0 > (connfd = accept(sockfd, (struct sockaddr*)&c_addr, &len)))
        slogf(ERROR, "Accept");

    event.data.fd = connfd;
    if(0 > epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connfd, &event))
        slogf(ERROR, "epoll_ctl");

    event.data.fd = fileno(stdin);
    if(0 > epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fileno(stdin), &event))
        slogf(ERROR, "epoll_ctl");

    events = (epoll_event*)calloc(10, sizeof(epoll_event));

    puts("waiting---");
    srand(time(NULL));
    int r = rand() & 0xffff00;
    while(1)
    {
        int rc = epoll_wait(epoll_fd, events, 10, 10);
        if(rc == -1)
            slogf(ERROR, "epoll_wait");
        for(int i = 0; i < rc ; ++i)
        {
            if((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || !(events[i].events & EPOLLIN) )
                slogf(ERROR, "disconnected");

            if(connfd == events[i].data.fd)
            {
                char buf[256] = {};
                int len = read(connfd, buf, 256);
                if(len == -1)slogf(INFO, "%s\n", strerror(errno));
                if(len == 0)exit(0);
                printf("Read from socket: %d --------\n", len);
                write(1, buf, len);
                puts(  "--------------------------");
            }

            if(fileno(stdin) == events[i].data.fd)
            {
                char buf[256] = {};
                fgets(buf, 256, stdin);
                if(buf[0] == '@') {
                     //aa 00 058003 22d0ee 0100 170605 203041 5f 0b
                    for(int i = 0; i < 20; ++i) {
                        char line[40] = {};
                        time_t raw;
                        time(&raw);
                        raw += rand() % 300;
                        struct tm *t = localtime(&raw);
                        sprintf(line, "aa00058003%06x0100170607%02d%02d%02d0000\r\n", r+i, t->tm_hour, t->tm_min, t->tm_sec);
                        printf("sending: %s\n", line);
                        write(connfd, line, strlen(line));
                    }
                } else {
                    int len = 0;
                    for(; len < 255; ++len) {
                        if(buf[len] == '\n') {
                            buf[len] = '\r';
                            buf[len+1] = '\n';
                            break;
                        }
                    }
                    write(connfd, buf, len+2);
                }
            }
        }
    }
    
    return 0;
}

