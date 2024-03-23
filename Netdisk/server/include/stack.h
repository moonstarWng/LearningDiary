#ifndef STACK_H
#define STACK_H

typedef struct pathstack_s{
    char* home;
    char* username;
    char* path[100];
    int length;
}pathstack_t;

//返回当前虚拟路径
char* virtual_path_get(pathstack_t* stack);
//返回当前绝对路径
char* path_get(pathstack_t* stack);
//栈初始化，需要传入登录用户名
int stack_init(pathstack_t* stack,char* username);
//入栈
int stack_push(pathstack_t* stack,char* filename);
//出栈
int stack_pop(pathstack_t* stack);

#endif
