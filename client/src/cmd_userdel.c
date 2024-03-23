#include "../include/main.h"

int cmd_userdel_c(int netfd){
    //初始化小火车
    train_t train;
    bzero(train.data,sizeof(train.data));
    printf("are you sure delete user? Y/N\n");
//    //接收来自服务端的注销用户指令反馈
//    recv(netfd,&train.len,sizeof(train.len),0);
//    recv(netfd,&train.data,train.len,0);
//    //将火车中的内容放入输出缓冲区中
//    puts(train.data);
    //读取stdin中的内容
    char buf[20];
    while(1){
        bzero(buf,sizeof(buf));
        int ret=read(STDIN_FILENO,buf,sizeof(buf));
        buf[ret-1]='\0';
        if(strcmp(buf,"N")==0){
            printf("userdel cancel!\n");
            train.len=strlen(buf);
            strcpy(train.data,buf);
            send(netfd,&train,sizeof(train.len)+train.len,0);
            break;
        }
        else if(strcmp(buf,"Y")==0){
            train.len=strlen(buf);
            strcpy(train.data,buf);
            send(netfd,&train,sizeof(train.len)+train.len,0);
            recv(netfd,&train.len,sizeof(train.len),0);
            recv(netfd,&train.data,train.len,0);
//           printf("1\n");
            //如果反馈为1，注销失败
            if(strcmp(train.data,"1")==0){
                printf("usredel fail！\n");
                return 1;
            }
            else{
                printf("userdel succeed!\n");
                close(netfd);
                exit(0);
            }
        }
        else{
            printf("erro cmd please try again！Y/N\n");
            continue;
        }
    }
    return 0;
}
