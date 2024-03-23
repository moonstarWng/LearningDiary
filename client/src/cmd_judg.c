#include "../include/main.h"
/*判断输入是否 合理 并且  将命令行参数作为指针返回 :
*   1、将输入分为  命令和参数
*   2、判断命令是否合理 “cd ls puts gets remove pwd mkdir rmdir”
*       2.1判断参数是否合理（没有，有一个）
*        没有参数的命令：ls pwd  
*      有一个参数的命令 ： cd puts  gets  remove  mkdir rmdir  
*/

int com_judg(char* command){
    //1、切割
    char *pcmd = strtok(command," ");
    if(pcmd==NULL)
    {
      printf("命令输入不合理\n");
      return -1;
    }
    //判断命令合理性
    if(strcmp(pcmd ,"ls")!=0 && strcmp(pcmd ,"pwd")!=0 
       && strcmp(pcmd ,"mkdir")!=0 && strcmp(pcmd ,"rmdir")!=0 
       && strcmp(pcmd ,"rm")!=0 && strcmp(pcmd ,"cd")!=0
       && strcmp(pcmd ,"userdel")!=0 && strcmp(pcmd ,"puts")!=0 && strcmp(pcmd ,"gets")!=0){
          printf("命令输入不合理\n");
          return -1;
       }
    else{
       char *parg = strtok(NULL," ");
       //为其他命令，需判断，无参数错误、两个参数错误
       char *errarg = strtok(NULL," ");//分割看第二个参数是否存在
       //命令为ls 、pwd则无参数
       if(strcmp(pcmd,"userdel")==0 || strcmp(pcmd,"ls")==0 || strcmp(pcmd,"pwd")==0){
         if(parg != NULL){
            printf("1参数输入不合理\n");
            return -1;
         }
        }

      //命令不为ls 和 pwd
       else{
         //输入的参数为空或第二个参数不为空,则出错
          if(parg == NULL || errarg != NULL){
            printf("2参数输入不合理\n");
            return -1;
          }
          else{ 
            //输入的参数合理，获取参数的长度,为命令申请大小，strcpy到*p里面
            size_t length = strlen(parg);
            //printf("1\n");
          }
        }
    }
    return 0;
}