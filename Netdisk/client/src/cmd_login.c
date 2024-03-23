#include "../include/main.h"

int cmd_signup_user(int sockfd, char* user, char *password){
    train_t train;
    // 输入用户名
    // 命令行提示
    // 从 stdin 获取用户名并传入小火车
    memset(train.data,0,sizeof(train.data));
    // 小火车发送用户名
    strcpy(train.data, user);
    train.len = strlen(train.data);
    send(sockfd, &train, sizeof(train.len)+strlen(train.data),MSG_NOSIGNAL);
    // 小火车发送密码
    memset(train.data,0,sizeof(train.data));
    // 小火车发送用户密码
    strcpy(train.data, password);
    train.len = strlen(train.data);
    send(sockfd, &train, sizeof(train.len)+strlen(train.data),MSG_NOSIGNAL);
    char buf[2];
    recv(sockfd,&buf, 1,0);
    buf[2] = '\0';
    // 判断登录是否成功
    if(strcmp("0", buf) == 0)
    {   
        printf("注册成功！\n");
        return 0;
        // 登陆成功返回 0
    }   
    printf("注册失败，用户名重复!\n");
    exit(0);
    // 登陆失败返回 1
    return 1;
    
}


int cmd_login_user(int sockfd, char* user, char *password){
    train_t train;
    // 从 stdin 获取用户名并传入小火车
    printf("user = %s\n",user);
    printf("password = %s\n",password);
    memset(train.data,0,sizeof(train.data));
    // 小火车发送用户名
    strcpy(train.data, user);
    train.len = strlen(train.data);
    send(sockfd, &train, sizeof(train.len)+strlen(train.data),MSG_NOSIGNAL);
    // 从 stdin 获取密码并传入小火车
    memset(train.data,0,sizeof(train.data));
    // 小火车发送用户密码
    strcpy(train.data, password);
    train.len = strlen(train.data);
    send(sockfd, &train, sizeof(train.len)+strlen(train.data),MSG_NOSIGNAL);
    // 接收服务端反馈，如果返回数据为"1"则登陆成功
    char buf[2];
    recv(sockfd,&buf, 1,0);
    buf[2] = '\0';
    // 判断登录是否成功
    if(strcmp("0", buf) == 0)
    {   
        printf("登录成功！\n");
        return 0;
        // 登陆成功返回 0
    }   
    printf("登陆失败!\n");
    exit(0);
    // 登陆失败返回 1
    return 1;
}
