#include "../include/main.h"

//返回虚拟路径字符串，pwd调用
char* virtual_path_get(pathstack_t* stack)
{
    if(stack->length==0)
    {
        return "/";
    }
    long count=0;
    for(int i=0;i<stack->length;i++)
    {
        count+=strlen(stack->path[i]);
    }
    
    char *p=(char*)calloc(1,count);
    for(int i=0;i<stack->length;i++)
    {
        strcat(p,stack->path[i]);
    }
    return p;
}

//返回绝对路径字符串，pwd调用
char* path_get(pathstack_t* stack)
{
    long count=0;
    count+=strlen(stack->home);
    count+=strlen(stack->username);
    for(int i=0;i<stack->length;i++)
    {
        count+=strlen(stack->path[i]);
    }
    char *p=(char*)calloc(1,count);
    strcpy(p,stack->home);
    strcat(p,stack->username);
    for(int i=0;i<stack->length;i++)
    {
        strcat(p,stack->path[i]);
    }
    return p;
}

//初始化栈，定义home和username
int stack_init(pathstack_t* stack,char* username)
{
    stack->home=(char*)calloc(1,5);
    strcpy(stack->home,"/home");
    
    stack->username=(char*)calloc(1,sizeof(username)+1);
    strcpy(stack->username,"/");
    strcat(stack->username,username);

    for (int i = 0; i < 100; i++) 
    {
        stack->path[i] = NULL;
    }
    stack->length=0;
    return 0;
}

//入栈，cd中调用
int stack_push(pathstack_t* stack,char* filename)
{
    stack->path[stack->length]=(char*)calloc(1,strlen(filename)+1);
    strcpy(stack->path[stack->length],"/");
    strcat(stack->path[stack->length],filename);
    printf("插入栈成功！插入栈中的值为：%s,%d\n",stack->path[stack->length],stack->length+1);
    stack->length+=1;
    return 0;
}

//出栈，cd中调用
int stack_pop(pathstack_t* stack)
{
    free(stack->path[stack->length-1]);
    stack->path[stack->length-1]=NULL;
    stack->length-=1;
    printf("出栈成功,%d\n",stack->length);
    return 0;
}

