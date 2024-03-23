#include "../include/main.h"
/*1、rm 删除文件
在当前路径下是否存在该文件，
存在删除，
不存在返回错误信息

参数设计：数据库的指针MYSQL*mysql、接收到的信息（netfd、arg）、返回的状态（执行成功、执行失败）
退出情况：
    文件不存在，退出
    删除成功，退出
*/
int cmd_rm(MYSQL*mysql,int netfd,char*filename, char *user, char* pathname){
       //拼接文件路径
       //char filepath[1024];
       char stat[1024];
    //    if(strcmp(pathname,"/")==0)
    // {
    //     sprintf(filename,"%s%s",pathname,filename);
    // }
    // else
    // {
    //     sprintf(filename,"%s/%s",pathname,filename);
    // }
       //printf("path = %s, file = %s\n",pathname, filename);
       //printf("filepath = %s,user = %s\n",filepath, user);
       //判断文件是否存在，:在数据库中判断，user path  type  tomb 来判断文件是否存在
       //不存在返回错误信息，退出
       char query_cmd[4096] = {0};
       printf("path = %s;\n", pathname);
       sprintf(query_cmd, "select * from files where user = '%s' and path = '%s' and filename = '%s'"
                        " and type = '-' and tomb = 0;"
                         ,user,pathname, filename);
       int qret1 = mysql_query(mysql,query_cmd);
       MYSQL_RES* res1 = mysql_use_result (mysql);
       MYSQL_ROW row1;
       //printf("rm 1\n");
       row1 = mysql_fetch_row(res1); 
       // char* id = row1[0];
       // printf("id = %s\n", id);
       //不存在这行，则报错，并退出
       if(row1 == NULL){
            strcpy(stat,"file is not exist");
            send(netfd,stat,sizeof(stat),MSG_NOSIGNAL);
            //printf("rm 不存在该行\n");
            return 1;
       }else{
          //存在该文件，则删除，即修改tomb为1
          memset(query_cmd,0,sizeof(query_cmd));
          sprintf(query_cmd,"update files set tomb = 1 where"
                  " user = '%s' and path = '%s' and filename = '%s' and type = '-';"
                  ,user,pathname, filename);
       }
       printf("query = %s\n",query_cmd);
       mysql_free_result(res1);
       int qret2 = mysql_query(mysql,query_cmd);
       strcpy(stat,"file delete successfully");
       send(netfd,stat,sizeof(stat),MSG_NOSIGNAL);
       //printf("rm 存在修改tomb为1\n");
       return 0; 
}



/*2、rmdir  删除空目录
在当前路径下是否存在该目录，
存在：该目录是否为空（需要打开目录），空目录删除，
目录非空返回错误信息，

参数设计：当前路径、接收到的信息（netfd，arg），返回状态（删除成功、删除失败）
*/
int cmd_rmdir(MYSQL*mysql,int netfd,char*dirname, char *user,char* pathname){
     //拼接目录的路径
    char dirpath[1024];
    char stat[1024];
    if(strcmp(pathname,"/")==0)
    {
        sprintf(dirpath,"%s%s",pathname,dirname);
    }
    else
    {
        sprintf(dirpath,"%s/%s",pathname,dirname);
    }
       //判断文件是否存在，:在数据库中判断，user path  type  tomb 来判断文件是否存在
       //不存在返回错误信息，退出
    printf("path = %s\n",dirpath); 
    
    char query_cmd[4096] = {0};
    //该sql语句：查询该目录是否存在，
    //（目录不存在：返回行数为 0 （删除失败，该目录不存在）  
    //非空目录：返回多行（删除失败，该目录不为空））
    //空目录：只返回一行（删除成功，修改tom为1）
    sprintf(query_cmd," select id from files  where user ='%s' "
                    "  and tomb = 0 and  path = '%s' AND type = 'd';"
                     ,user,dirpath);
    printf("user = %s, path = %s;\n",user, dirpath);
    int qret1 = mysql_query(mysql,query_cmd);
    MYSQL_RES* res1 = mysql_use_result(mysql);
    MYSQL_ROW row1 = mysql_fetch_row(res1);
    if(row1 == NULL){
        strcpy(stat,"Dir not exist");
        send(netfd,stat,sizeof(stat),MSG_NOSIGNAL);
        return 1;
    }
    char pre_id[100];
    strncpy(pre_id, row1[0],sizeof(pre_id));
    mysql_free_result(res1);
    printf("pre_id = %s\n",pre_id);
    sprintf(query_cmd,"select * from files where user ='%s' "
                    "  and tomb = 0 and pre_id = '%s';"
                     ,user,pre_id);
    int qret2 = mysql_query(mysql,query_cmd);
    MYSQL_RES* res2 = mysql_use_result(mysql);
    MYSQL_ROW row2 = mysql_fetch_row(res2);
    if(row2 != NULL){
        strcpy(stat,"Dir not NULL");
        send(netfd,stat,sizeof(stat),MSG_NOSIGNAL);
        mysql_free_result(res2);
        return 1;
    }
    memset(query_cmd,0,sizeof(query_cmd));
    sprintf(query_cmd,"update files  set tomb = 1  where user = '%s' and path = '%s' and type = 'd';"
                      ,user,dirpath);
    int qret3 = mysql_query(mysql,query_cmd);
    strcpy(stat,"rmdir success");
    send(netfd,stat,sizeof(stat),MSG_NOSIGNAL);
    return 0;
    }


int cmd_mkdir(MYSQL*mysql,int netfd,char*dirname, char *user,char *pathname){
    char dirpath[1024];
    char stat[1024];
    if(strcmp(pathname,"/")==0)
    {
        sprintf(dirpath,"%s%s",pathname,dirname);
    }
    else
    {
        sprintf(dirpath,"%s/%s",pathname,dirname);
    }
    char sql[4096] = {0};
    printf("path = %s, file = %s\n",pathname, dirname);
    printf("path = %s\n",dirpath); 
    //1、查询目录是否存在
    sprintf(sql,"select * from files where user = '%s'"
        " and type = 'd' and tomb = 0 and path = '%s';"
        ,user,dirpath);
    int qret1 = mysql_query(mysql,sql);
    MYSQL_RES *res1 = mysql_use_result(mysql); 
    MYSQL_ROW row1 = mysql_fetch_row(res1);
    printf("mkdir1\n");
    if( row1 != NULL){//存在，查询到结果不为空，则退出
    printf("2\n");
        strcpy(stat,"Dir  exist");
        send(netfd,stat,sizeof(stat),MSG_NOSIGNAL);
        printf("3\n");
        mysql_free_result(res1);
        return 1;
    }
    else{
    printf("4\n");
        memset(sql,0,sizeof(sql));
        sprintf(sql," select * from files where user = '%s' "
                "and type = 'd' and tomb = 1 and path = '%s';"
                     ,user,dirpath);
        int qret2 = mysql_query(mysql,sql);
        MYSQL_RES *res2 = mysql_use_result(mysql); 
        MYSQL_ROW row2 = mysql_fetch_row(res2); 
        //不存在有两种情况：tomb = 1 存在 update /  tomb =1  不存在，insert into
        if(row2 != NULL){ //说明存在tomb为1的
           printf("5\n");
           memset(sql,0,sizeof(sql));
           sprintf(sql," update files set tomb = 0 where user = '%s'"
                   " and path = '%s' and type = 'd';"
                   ,user,dirpath);
           mysql_free_result(res2);
           mysql_query(mysql,sql);
           strcpy(stat,"mkdir success");
           send(netfd,stat,sizeof(stat),MSG_NOSIGNAL);
           return 0; 
        }
        else{//不存在 insert into 
            //先查找pre_id
           memset(sql,0,sizeof(sql));
           sprintf(sql,"select id from  files where  user = '%s'"
                   " and path = '%s' and tomb = 0;"
                   ,user, pathname);
           int qret3 = mysql_query(mysql,sql);
           MYSQL_RES* res3 = mysql_use_result(mysql);
           MYSQL_ROW row3 = mysql_fetch_row(res3);
           memset(sql,0,sizeof(sql));
            char pre_id[20]="1";
            if(row3 != NULL){
                strcpy(pre_id,row3[0]);
            }

            sprintf(sql," insert into files "
                    "(filename,user,pre_id,path,type,sha1sum,size,tomb)"
                    " values ('%s','%s',%s,'%s','d','1234',0,0);",
                               dirname,user,pre_id,dirpath);
            printf("6\n");
           mysql_free_result(res3);
            printf("6\n");
           mysql_query(mysql,sql);
            printf("6\n");
        }
    }
    strcpy(stat,"mkdir success");
    send(netfd,stat,sizeof(stat),MSG_NOSIGNAL);
    return 0;
}
