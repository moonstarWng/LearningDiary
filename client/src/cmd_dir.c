#include "../include/main.h"

//客户端ls
int cmd_ls(int sockfd)
{
    train_t train;
    memset(train.data,0,sizeof(train.data));
    //接收服务端是否成功的消息
    recv(sockfd,&train.len,sizeof(long),0);
    recv(sockfd,train.data,train.len,0);
    //成功 x
    if(strcmp(train.data,"0")==0)
    {
        //接收文件路径信息
        bzero(train.data,sizeof(train.data));
        int ret=recv(sockfd,&train.len,sizeof(train.len),0);
        if(ret==0)
        {
            printf("ret==0\n");
            //break;
        }
        recv(sockfd,train.data,train.len,0);
        puts(train.data);
        return 0;
    }
    //失败
    else if(strcmp(train.data,"1")==0)
    {
        return 1;
    }
    return 1;
}

//客户端pwd
int cmd_pwd(int sockfd)
{
    train_t train;
    memset(train.data,0,sizeof(train.data));
    //接收服务端是否成功的消息
    recv(sockfd,&train.len,sizeof(long),0);
    recv(sockfd,&train.data,train.len,0);

    //成功
    if(strcmp(train.data,"0")==0)
    {
        //接收文件路径信息
        bzero(train.data,sizeof(train.data));
        recv(sockfd,&train.len,sizeof(train.len),0);
        recv(sockfd,train.data,train.len,0);
        puts(train.data);
        return 0;
    }
    //失败
    else if(strcmp(train.data,"1")==0)
    {
        bzero(train.data,sizeof(train.data));
        recv(sockfd,&train.len,sizeof(train.len),0);
        recv(sockfd,train.data,train.len,0);
        puts(train.data);
        return 1;
    }
    return 1;
}

//客户端cd
int cmd_cd(int sockfd)
{
    train_t train;
    memset(train.data,0,sizeof(train.data));
    //接收服务端是否成功的消息
    recv(sockfd,&train.len,sizeof(long),0);
    recv(sockfd,train.data,train.len,0);
    //成功
    if(strcmp(train.data,"0")==0)
    {
        //接收文件路径信息
        memset(train.data,0,sizeof(train.data));
        recv(sockfd,&train.len,sizeof(train.len),0);
        recv(sockfd,train.data,train.len,0);
        puts(train.data);
        return 0;
    }
    //失败
    else if(strcmp(train.data,"1")==0)
    {
        memset(train.data,0,sizeof(train.data));
        recv(sockfd,&train.len,sizeof(train.len),0);
        recv(sockfd,train.data,train.len,0);
        puts(train.data);
        return 1;
    }
    return 1;
}
