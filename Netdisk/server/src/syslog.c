#include "../include/main.h"
//user为用户名，cmd为发送的命令字符串，ret为返回的结果，
//查看日志cat /var/log/syslog
void write_log(char* user,char* cmd,int ret,char* file,const char* func, int line)
{
    char pcmd[1024];
    strcpy(pcmd,cmd);//暂存命令字符串，以免分割后不完整
    char *tcmd=strtok(cmd," ");//分割命令字符串
    openlog("Netdisklog", LOG_PID, LOG_USER);
    if(strcmp(tcmd,"login")==0)//判断是否为登录
    {
        if(ret==0)//登录成功
        {
            syslog(LOG_INFO,"%s文件 %s函数 第%d行 用户%s登录成功\n",file,func,line,user);
        }
        else//登录失败
        {
            syslog(LOG_INFO,"%s文件 %s函数 第%d行 用户%s登录失败\n",file,func,line,user);
        }
    }
    //判断是否为命令字符串
    else if(strcmp(tcmd,"cd")==0||strcmp(tcmd,"ls")==0||strcmp(tcmd,"pwd")==0||strcmp(tcmd,"rm")==0||strcmp(tcmd,"mkdir")==0||strcmp(tcmd,"rmdir")==0||strcmp(tcmd,"puts")==0||strcmp(tcmd,"gets")==0)
    {
        if(ret==0)//命令执行成功
        {
            syslog(LOG_INFO,"%s文件 %s函数 第%d行 用户%s执行 %s 命令成功\n",file,func,line,user,pcmd);
        }
        else//命令执行失败
        {
            syslog(LOG_INFO,"%s文件 %s函数 第%d行 用户%s执行 %s 命令失败\n",file,func,line,user,pcmd);
        }
    }
    else//命令非法
    {
        syslog(LOG_INFO,"%s文件 %s函数 第%d行 用户%s执行 %s 命令非法！！！\n",file,func,line,user,pcmd);
    }
    closelog();
}


// int main()
// {
//     int ret=0;
//     char cmd[]="ld ./dir";
//     char user[]="study";
//     printf("hello\n");
//     LOG(user,cmd,ret);
//     LOG(user,"login",ret);
//     printf("bye bye\n");
//     return 0;
// }
