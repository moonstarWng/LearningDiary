#include "../include/main.h"

//添加目标监听fd到epfd中，只会返回0
int epoll_add(int epfd,int fd)
{
   struct epoll_event events;
   events.events=EPOLLIN;
   events.data.fd=fd;
   epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&events);
   return 0;
}

//将fd从监听集合epfd中移除，只会返回0
int epoll_del(int epfd,int fd)
{
    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
    return 0;
}
