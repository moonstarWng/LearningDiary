#include "../include/main.h"

//服务端ls
int cmd_ls(int sockfd,pathstack_t* stack,MYSQL* mysql,char* user)
{
    train_t train;
    //查找当前目录虚拟路径在表中的id值
    char sql[4096];
    memset(sql,0,sizeof(sql));
    sprintf(sql,"select id from files where user='%s' and path='%s' and tomb=0;",user,virtual_path_get(stack));
    mysql_query(mysql, sql); 

    //将结果保存在row中
    MYSQL_RES *result = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(result);
    int id=-1;
    if (row) {
        id=atoi(row[0]);
    }
    else
    {
        //发1失败
        train.len=1;
        memset(train.data,0,sizeof(train.data));
        strcpy(train.data,"1");
        send(sockfd,&train,sizeof(train.len)+strlen(train.data),0);
        mysql_free_result(result);
        return 1;
    }
    //第一堂小火车，发0成功
    train.len=1;
    memset(train.data,0,sizeof(train.data));
    strcpy(train.data,"0");
    send(sockfd,&train,sizeof(train.len)+strlen(train.data),0);

    mysql_free_result(result);
    //根据id查找当前虚拟路径下的文件
    memset(sql,0,sizeof(sql));
    sprintf(sql,"select filename from files where user='%s' and pre_id='%d'and tomb=0;",user,id);
    mysql_query(mysql, sql); 

    //接收mysql查询结果
    MYSQL_RES *res = mysql_store_result(mysql);
    memset(train.data,0,sizeof(train.data));
    while((row = mysql_fetch_row(res)) != NULL){
        for(unsigned int i = 0; i < mysql_num_fields(res); ++i){
            strcat(train.data,row[i]);
            strcat(train.data,"   ");
        }
    }
    strcat(train.data,"\n");
    train.len=strlen(train.data);
    send(sockfd,&train,sizeof(train.len)+strlen(train.data),0);
    mysql_free_result(res);
    return 0;
}

//服务端pwd
int cmd_pwd(int sockfd,pathstack_t* stack)
{
    train_t train;
    //先发一个1表示正常发数据
    train.len=1;
    memset(train.data,0,sizeof(train.data));
    strcpy(train.data,"0");
    send(sockfd,&train,sizeof(train.len)+strlen(train.data),0);

    //第二趟火车装pwd结果
    memset(train.data,0,sizeof(train.data));
    if(stack->length==0)
    {
        strcpy(train.data,"/");
    }
    else
    {
        for(int i=0;i<stack->length;i++)
        {
            strcat(train.data,stack->path[i]);
        }
    }
    train.len=strlen(train.data);
    send(sockfd,&train,sizeof(train.len)+strlen(train.data),0);
    return 0;
}

//服务端cd
int cmd_cd(int sockfd,pathstack_t* stack,MYSQL* mysql,char* name,char* user)
{   
    char filename[256];
    memset(filename,0,sizeof(filename));
    strcpy(filename,name);
    train_t train;
    pathstack_t temp;
    temp.length=stack->length;
    for(int i=0;i<stack->length;i++)
    {
        temp.path[i]=(char*)calloc(1,strlen(stack->path[i]));
        strcpy(temp.path[i],stack->path[i]);
    }

    char p[256];
    memset(p,0,sizeof(p));
    for(int i=0;i<strlen(filename);i++)
    {
        if(filename[i]=='/')
        {
           strcpy(p,strtok(filename,"/")); 
           break;
        }
        else if(i==strlen(filename)-1)
        {
            strcpy(p,filename);
        }
    }
    printf("准备开始#####%s#####\n",p);

    while(p!=NULL)
    {
        printf("开始判断！\n");
        //.
        if(strcmp(p,".")==0)
        {
            char* token = strtok(NULL, "/");
            if (token != NULL) {
                strcpy(p, token);
            } else {
                train.len=1;
                memset(train.data,0,sizeof(train.data));
                strcpy(train.data,"0");    
                send(sockfd,&train,sizeof(train.len)+train.len,0);
                    //发送当前虚拟路径给客户端
                memset(train.data,0,sizeof(train.data));
                strcpy(train.data,virtual_path_get(stack));
                train.len=strlen(train.data);
                send(sockfd,&train,sizeof(train.len)+train.len,0);
                for(int i=0;i<temp.length;i++)
                {
                    free(temp.path[i]);
                }
                return 0;
            }
            memset(p,0,sizeof(p));
            strcpy(p,token);
            continue;
        }

        //..
        if(strcmp(p,"..")==0)
        {
            //在根目录下调用cd ..
            if(temp.length==0)
            {
                train.len=1;
                strcpy(train.data,"1");
                send(sockfd,&train,sizeof(train.len)+train.len,0);
                memset(train.data,0,sizeof(train.data));

                strcpy(train.data,"当前已经位于根目录！\n");
                train.len=strlen(train.data);
                send(sockfd,&train,sizeof(train.len)+strlen(train.data),0);
                for(int i=0;i<temp.length;i++)
                {
                    free(temp.path[i]);
                }
                return 1;
            }
            stack_pop(&temp);
            memset(p,0,sizeof(p));
            char* token = strtok(NULL, "/");
            if (token != NULL) {
                strcpy(p, token);
                continue;
            } else {
                break;
            }
        }

        //入栈
        stack_push(&temp,p);
        char*path=virtual_path_get(&temp);
        printf("while到底\n");
        //查文件表，看文件是否存在
        char sql[4096];
        memset(sql,0,sizeof(sql));
        sprintf(sql,"select * from files where path='%s' and user='%s' and tomb=%d and type='%s';",path,user,0,"d");
        mysql_query(mysql, sql); 
        printf("while到底\n");
        //接收结果
        MYSQL_RES *result = mysql_store_result(mysql);
        int num_rows = mysql_num_rows(result);
        if(num_rows==0)
        {            
            train.len=1;
            strcpy(train.data,"1");
            send(sockfd,&train,sizeof(train.len)+train.len,0);
            
            memset(train.data,0,sizeof(train.data));
            strcpy(train.data,"路径有误！\n");
            train.len=strlen(train.data);
            send(sockfd,&train,sizeof(train.len)+train.len,0);

            mysql_free_result(result);
            for(int i=0;i<temp.length;i++)
            {
                free(temp.path[i]);
            }
            return 1;
        }
        mysql_free_result(result);
        printf("while到底\n");
        //切割
        memset(p,0,sizeof(p));

        char* token = strtok(NULL, "/");
        if (token != NULL) {
            strcpy(p, token);
        } else {
            break;
        }
        printf("while到底\n");

    }
    printf("判断成功！\n");
    //出了while循环就代表成功
    train.len=1;
    memset(train.data,0,sizeof(train.data));
    strcpy(train.data,"0");    
    send(sockfd,&train,sizeof(train.len)+train.len,0);
    
    //目录存在，拷贝temp栈到stack栈，并释放temp栈
    for(int i=0;i<stack->length;i++)
    {
        free(stack->path[i]);
        stack->path[i]=NULL;
    }
    for(int i=0;i<temp.length;i++)
    {
        stack->path[i]=(char*)calloc(1,strlen(temp.path[i]));
        strcpy(stack->path[i],temp.path[i]);
        free(temp.path[i]);
    }
    stack->length=temp.length;

    //发送当前虚拟路径给客户端
    memset(train.data,0,sizeof(train.data));
    strcpy(train.data,virtual_path_get(stack));
    train.len=strlen(train.data);
    send(sockfd,&train,sizeof(train.len)+train.len,0);

    return 0;
}
