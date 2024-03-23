#include "../include/main.h"

int cmd_userdel_s(int netfd,char *username,MYSQL* mysql)
{   //初始化小火车
    train_t train;
    bzero(train.data,sizeof(train.data));
    //发送用户注销确认信息
//    char *str="are you sure delete user? Y/N";
//    strcpy(train.data,str);
//    send(netfd,&train,sizeof(train.len)+strlen(train.data),0);
//    bzero(train.data,sizeof(train.data));
//    //接收来自用户的注销确认返回信息
    recv(netfd,&train.len,sizeof(train.len),0);
    recv(netfd,&train.data,train.len,0);
    if(strcmp(train.data,"Y")==0){
        //将用户表中的tomb位置为1表示该用户已经注销
        train.len=1;
        char sql[225];
        snprintf(sql,225,"update users set tomb = 1 where username = '%s';",username); 
        int qret = mysql_query(mysql,sql);
        //当sql语句执行失败时往tcp连接中写入1
        if(qret != 0){
            fprintf(stderr,"mysql_query:%s\n", mysql_error(mysql));
            strcpy(train.data,"1");
            send(netfd,&train,sizeof(train.len)+strlen(train.data),0);
            return 1;
        }
        //当执行成功时往tcp中写入0
        bzero(train.data,sizeof(train.data));
        strcpy(train.data,"0");
        send(netfd,&train,sizeof(train.len)+train.len,0);
        return 0;
    }
    else{
        return 1;
    }
        
}

