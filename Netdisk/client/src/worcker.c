#include "../include/main.h"

//主进程传入的结构体
typedef struct pthread_data_s{
    char* user;
    char* token;
    char* cmd;
    char* ip;
    char* port;
}pthread_data_t;

//和服务端的链接协议
static int agreement(pthread_data_t* data,int sockfd)
{
    //发1
    train_t train;
    train.len=1;
    memset(train.data,0,sizeof(train.data));
    strcpy(train.data,"1");
    send(sockfd,&train,sizeof(train.len)+train.len,0);
    //发user
    train.len=strlen(data->user);
    memset(train.data,0,sizeof(train.data));
    strcpy(train.data,data->user);
    send(sockfd,&train,sizeof(train.len)+train.len,0);
        //发user
    train.len=strlen(data->user);
    memset(train.data,0,sizeof(train.data));
    strcpy(train.data,data->user);
    send(sockfd,&train,sizeof(train.len)+train.len,0);
    //发token
    train.len=strlen(data->token);
    memset(train.data,0,sizeof(train.data));
    strcpy(train.data,data->token);
    send(sockfd,&train,sizeof(train.len)+train.len,0);
    //获取身份验证的回复
    memset(train.data,0,sizeof(train.data));
    recv(sockfd,&train.len,sizeof(train.len),0);
    recv(sockfd,train.data,train.len,0);
    if(strcmp(train.data,"0")==0)
    {
        //发命令
        train.len=strlen(data->cmd);
        memset(train.data,0,sizeof(train.data));
        strcpy(train.data,data->cmd);
        send(sockfd,&train,sizeof(train.len)+train.len,0);
        return 0;
    }
    else
    {
        return 1;
    }
}

void * threadFunc(void *arg)
{
    //接收传参，定义小火车
    pthread_data_t* data=(pthread_data_t*)arg;
    train_t train;

    //和服务端建立连接
    int sockfd;
    int tcp_ret=tcp_init(data->ip,data->port,&sockfd);
    if(tcp_ret==1)
    {
        printf("出现错误、线程与服务端建立tcp链接失败！\n");
        return NULL; 
    }

    //封装函数：用来和服务端沟通的协议
    int ret=agreement(data,sockfd);
    if(ret==1)
    {
        printf("出现错误、下载/上传文件时用户身份未核验通过！\n");
        return NULL;
    }

    //拷贝一份命令给cutcmd，它专门用来被切割，防止data损坏
    char* cutcmd=(char*)calloc(1,strlen(data->cmd));
    strcpy(cutcmd,data->cmd);
    
    //切割命令，fir_cmd用来判断是puts还是gets，sec_cmd用来给参数传参
    char* fir_cmd=strtok(cutcmd," ");
    char* sec_cmd=strtok(NULL," ");
    //根据puts、gets调用不同执行命令函数
    printf("子线程开始下载\n");
    if(strcmp(fir_cmd,"puts")==0){
        cmd_puts_in_client(sockfd,sec_cmd);}
    else if(strcmp(fir_cmd,"gets")){cmd_gets_in_client(sockfd,sec_cmd);}
   
    sleep(5);
    printf("子线程下载完成\n");
    free(cutcmd);
    return NULL;
}
