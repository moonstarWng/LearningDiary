#include "../include/main.h"

//参数1：sockfd，参数2：命令前缀
int client_ready(int sockfd,char fir_cmd[])
{
    if(strcmp(fir_cmd,"cd")==0){cmd_cd(sockfd);}
    else if(strcmp(fir_cmd,"ls")==0){cmd_ls(sockfd);}
    else if(strcmp(fir_cmd,"rm")==0){cmd_rm(sockfd);}
    else if(strcmp(fir_cmd,"pwd")==0){cmd_pwd(sockfd);}
    else if(strcmp(fir_cmd,"mkdir")==0){cmd_mkdir(sockfd);}
    else if(strcmp(fir_cmd,"rmdir")==0){cmd_rmdir(sockfd);}
    else if(strcmp(fir_cmd,"userdel")==0){cmd_userdel_c(sockfd);}
    return 0;
}
