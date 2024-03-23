#include "../include/main.h"

int epollAdd(int epfd, int fd){
    struct epoll_event events;
    events.events = EPOLLIN;
    events.data.fd = fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&events);
    return 0;
}
int epollDel(int epfd, int fd){
    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
    return 0;
}
int filename_get(char *str,char **filename)
{
    char *temp=strtok(str," ");
    char *curr=temp;
    while(temp != NULL){
        curr=temp;
        temp=strtok(NULL,"/");
    }
    *filename=(char*)calloc(1,strlen(curr));
    strcpy(*filename,curr);
    return 0;
}
