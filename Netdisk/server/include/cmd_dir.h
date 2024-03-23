#ifndef CMD_DIR_H
#define CMD_DIR_H

//查看当前目录文件名
int cmd_ls(int sockfd,pathstack_t* stack,MYSQL* mysql,char* user);
//查看当前工作目录
int cmd_pwd(int sockfd,pathstack_t* stack);
//移动当前工作目录
int cmd_cd(int sockfd,pathstack_t* stack,MYSQL* mysql,char* filename,char* user);

#endif
