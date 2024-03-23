#include "../include/main.h"
char command[1024];
char token[1024];

//给线程传参用结构体
typedef struct pthread_data_s{
    char* user;
    char* token;
    char* cmd;
    char* ip;
    char* port;
}pthread_data_t;

//客户端服务端沟通协议
static void first_talk(int sockfd)
{
    train_t train;
    train.len=1;
    memset(train.data,0,sizeof(train.data));
    strcpy(train.data,"0");
    send(sockfd,&train,sizeof(train.len)+train.len,MSG_NOSIGNAL);
}

//接收token
static void second_talk(int sockfd)
{
    train_t train;
    memset(train.data,0,sizeof(train.data));
    memset(token,0,sizeof(token));
    recv(sockfd,&train.len,sizeof(train.len),0);
    recv(sockfd,token,train.len,0);
}

//展示用户端输入命令外观
static void menu(char* username)
{
    printf("\033[1;31m");
    printf("亲爱的用户 %s, 请输入指令: ",username);
    printf("\033[0m");
    fflush(stdout);
    return;
}

//从键盘获取输入指令并存储在command中
static void enter_cmd(void)
{
    memset(command,0,sizeof(command));
    int ret=read(STDIN_FILENO,command,sizeof(command));
    command[ret-1]='\0';
    return;
}

int main(int argc,char* argv[])
{
    ARGS_CHECK(argc,3);

    char username[20];
    char password[20];
    int ans = 0;
    while(1){
        printf("login(登录)/signup(注册),请输入指令：");
        fflush(stdout);
        char message[100];
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';
        if(strcmp(message,"signup") == 0){ 
            // 0 表示 signup 注册
            printf("开始用户注册\n");
            printf("\033[1;34m");
            printf("请输入用户名：");
            printf("\033[0m");
            fflush(stdout);
            fgets(username,sizeof(username),stdin);
            printf("\033[1;34m");
            printf("请输入登陆密码：");
            printf("\033[0m");
            fflush(stdout);
            fgets(password,sizeof(password),stdin);
            break;
        }else if(strcmp(message,"login")==0){
            // 1 表示 login 登录
            printf("开始登陆\n");
            ans = 1;
            printf("\033[1;34m");
            printf("请输入用户名：");
            printf("\033[0m");
            fflush(stdout);
            fgets(username,sizeof(username),stdin);
            printf("\033[1;34m");
            printf("请输入登陆密码：");
            printf("\033[0m");
            fflush(stdout);
            fgets(password,sizeof(password),stdin);
            break;
        }   
    }
    //tcp连接准备工作，由tcp_init完成
    int sockfd;
    int tcp_ret=tcp_init(argv[1],argv[2],&sockfd);
    //ERROR_CHECK(tcp_ret,1,"tcp_init");

    //监听STDIN以及服务端消息
    int epfd=epoll_create(1);
    epoll_add(epfd,sockfd);
    epoll_add(epfd,STDIN_FILENO);

    //S、C端沟通协议
    first_talk(sockfd);

    //调用注册、登录封装函数
    if(ans == 0){
        send(sockfd, &ans, sizeof(int), MSG_NOSIGNAL);
        cmd_signup_user(sockfd, username, password);
    }else{
        send(sockfd, &ans, sizeof(int), MSG_NOSIGNAL);
        cmd_login_user(sockfd, username, password);
    }
    //接收token
    second_talk(sockfd);
    username[strcspn(username, "\n")] = '\0';
    while(1)
    {
        //展示用户端输入命令外观
        menu(username);
        
        //调用epollwait来监听STDIN和sockfd的消息
        struct epoll_event readyset[10];
        int ret=epoll_wait(epfd,readyset,10,-1);

        //遍历就绪队列
        for(int i=0;i<ret;i++)
        {
            //进入这个if则表示用户输入新指令了
            if(readyset[i].data.fd==STDIN_FILENO)
            {
                //从键盘获取输入指令并存储在command字符串中
                enter_cmd();

                //输入quit则客户端退出
                if(strcmp(command,"quit")==0)
                {
                    close(sockfd);
                    return 0;
                }
                //sendcmd用来存储整条命令
                char* sendcmd=(char*)calloc(1,strlen(command));
                strcpy(sendcmd,command);

                //checkcmd用来命令检测传入
                char* checkcmd=(char*)calloc(1,strlen(command));
                strcpy(checkcmd,command);

                //用来切割指令，fir_cmd为前缀，sec_cmd为后缀
                char* fir_cmd=strtok(command," ");
                char* sec_cmd=strtok(NULL," ");

                //由com_judg函数判断命令合法性，合法就发送，否则结束本次循环
                int temp=com_judg(checkcmd);
                if(temp==-1)
                {     
                    printf("输入命令格式有误！\n");
                    continue;
                }

                //puts、gets交由线程处理
                if(strcmp(fir_cmd,"gets")==0||strcmp(fir_cmd,"puts")==0)
                {
                    pthread_t tid;
                    pthread_data_t data;
                    data.user=calloc(1,strlen(username));
                    data.token=calloc(1,strlen(token));
                    data.cmd=calloc(1,strlen(sendcmd));
                    data.ip=calloc(1,strlen(argv[1]));
                    data.port=calloc(1,strlen(argv[2]));
                    strcpy(data.user,username);
                    strcpy(data.token,token);
                    strcpy(data.cmd,sendcmd);
                    strcpy(data.ip,argv[1]);
                    strcpy(data.port,argv[2]);
                    pthread_create(&tid, NULL, threadFunc, &data);
                    pthread_detach(tid);
                    continue;
                }
                cmd_send(sockfd,username);
                cmd_send(sockfd,token);

                train_t train;
                memset(train.data,0,sizeof(train.data));
                recv(sockfd,&train.len,sizeof(train.len),0);
                recv(sockfd,train.data,train.len,0);
                if(strcmp(train.data,"0")==0)
                {
                    //发送命令的函数
                    cmd_send(sockfd,sendcmd);
                    //调用接口函数
                    client_ready(sockfd,fir_cmd);
                }
                else 
                {
                    printf("token未通过！\n");
                    continue;
                }
            }
            //进入这个if表示在键盘未输入指令的情况下接收到服务端消息
            else if(readyset[i].data.fd==sockfd)
            {
                printf("epollwait时收到服务端消息！退出\n");
                close(sockfd);
                return 0;
            }
        }
    }
    close(sockfd);
    return 0;
}

