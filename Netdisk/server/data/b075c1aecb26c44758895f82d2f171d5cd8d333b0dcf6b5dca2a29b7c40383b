#include "../include/main.h"

//参数1，sockfd，参数2：命令
int cmd_send(int netfd, char comd[])
{
    train_t train;
    train.len = strlen(comd);
    memset(train.data,0,sizeof(train.data));
    strcpy(train.data,comd);
    // 发送命令 头长度+车厢长度
    send(netfd,&train,sizeof(train.len)+train.len,MSG_NOSIGNAL);
    return 0;
}