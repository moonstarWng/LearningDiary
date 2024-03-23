#include "../include/main.h"
//客户端对于 rm、rdmir、mkdir的实现，接收服务端相关的返回值，最后打印出消息
int cmd_rm(int sockfd){
    char stat[4096];
    recv(sockfd,stat,sizeof(stat),0);
    printf("%s\n",stat);
    return 0;
}

int cmd_mkdir(int sockfd){
    char stat[4096];
    recv(sockfd,stat,sizeof(stat),0);
    printf("%s\n",stat);
    return 0;
}

int cmd_rmdir(int sockfd){
    char stat[4096];
    recv(sockfd,stat,sizeof(stat),0);
    printf("%s\n",stat);
    return 0;
}
 
