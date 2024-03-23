#include "../include/main.h"

int tidArrInit(tidArr_t * ptidArr, int workerNum){
    // 申请内存 存储每个子线程的tid
    ptidArr->arr = (pthread_t *)calloc(workerNum,sizeof(pthread_t));
    ptidArr->workerNum = workerNum;
    return 0;
}


//子线程的具体执行函数
void * threadFunc(void *arg){
    threadPool_t * pthreadPool = (threadPool_t *)arg;
    char buf[4096]={0};
    train_t train;
    while(1){
        //子线程被创建完后进入阻塞态，等待子进程发netfd
        //初始化mysql
        MYSQL *mysql;
        
        pthread_mutex_lock(&pthreadPool->mutex);
        mysql_link(&mysql);
        pthread_mutex_unlock(&pthreadPool->mutex);
        int netfd;
        char *token;
        //暂时删除退出标志位
        while(pthreadPool->taskQueue.queueSize <= 0){
            pthread_cond_wait(&pthreadPool->cond, &pthreadPool->mutex);
        }

        //当退出标志位为1时代表着线程执行完任务时就该退出了
        //        if(pthreadPool->exitFlag == 1){
        //            printf("一条线程退出!\n");
        //            pthread_mutex_unlock(&pthreadPool->mutex);
        //            pthread_exit(NULL);
        //        }
        netfd = pthreadPool->taskQueue.pFront->netfd;
        printf("一条线程开始工作\n");
        deQueue(&pthreadPool->taskQueue);
        pthread_mutex_unlock(&pthreadPool->mutex);
        bzero(train.data,sizeof(train.data));
        //接收来自客户端的user
        recv(netfd,&train.len,sizeof(train.len),0);
        recv(netfd,train.data,train.len,0);
        
        char *user = (char *)calloc(1,strlen(train.data));
        strcpy(user, train.data);

        int cret;

        //接收来自客户端的token       
        recv(netfd,&train.len,sizeof(train.len),0);
        recv(netfd,train.data,train.len,0);
        printf("user = %s\n",user);
        printf("token = %s\n",train.data);
        check_token(user, train.data, &cret);
        printf("cret = %d\n",cret);
        if(cret==0){
            pathstack_t stack;
            stack_init(&stack,user);
            memset(train.data, 0, sizeof(train.data));
            train.len=1;
            train.data[0]='0';
            send(netfd,&train,sizeof(train.len)+train.len,0);
            //接收来自客户端的指令puts/gets
            memset(train.data, 0, sizeof(train.data));
            recv(netfd,&train.len,sizeof(train.len),0);
            recv(netfd,train.data,train.len,0);
            char str[100];
            strncpy(str, train.data, 100);
            char *filename;
            char *cmd=strtok(train.data," ");
            filename_get(str,&filename);
            char *pathname=virtual_path_get(&stack);
            printf("pathname = %s;\n",pathname);
            printf("filename = %s;\n",filename);
            printf("netfd = %d\n",netfd);
            printf("user = %s\n",user);
            if(strcmp(cmd,"puts")==0){
                printf("puts start!\n");
                int ret=cmd_puts_in_server(mysql,netfd,pathname,filename,user);
                LOG(user,cmd,ret);
                sleep(5);
                printf("puts over!\n");
            }
            else{
                int ret=cmd_gets_in_server(mysql,netfd,pathname,filename,user);
                LOG(user,cmd,ret);
                sleep(5);
                printf("gets over!\n");
            }
            free(user);
        }
        else{
            train.len=1;
            train.data[0]='1';
            send(netfd,train.data,train.len,0);
            close(netfd);
        }
    }
}
