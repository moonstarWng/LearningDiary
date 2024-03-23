#include "../include/main.h"


int main(int argc, char *argv[])
{
    // ./server 192.168.72.128 1234 4
    ARGS_CHECK(argc,4);
    // 只有子进程 才能创建线程池，对线程池初始化
    // close(exitPipe[1]);
    threadPool_t threadPool;
    threadPoolInit(&threadPool, atoi(argv[3]));
    makeWorker(&threadPool);
    MYSQL *mysql;
    pthread_mutex_lock(&threadPool.mutex);
    mysql_link(&mysql);
    pthread_mutex_unlock(&threadPool.mutex);
    // 初始化tcp连接socket、bind、listen
    int sockfd;
    tcpInit(argv[1],argv[2],&sockfd);
    // 创建epoll,监听客户端来的信号以及父进程递送的退出信号
    int epfd = epoll_create(1);
    epollAdd(epfd,sockfd);
    // 时间轮初始化
    // 1. 声明时间轮数组，用于存储每个时间槽链表
    // 2. timeWheelIdx 存储当前时间轮下标，随时间更新
    // 3. 每个时间槽链表
    timeSlot_t *timeWheel[WHEELSIZE];
    int timeWheelIdx = 0;
    // 每个时间轮槽置空
     
    for(int i = 0; i < WHEELSIZE; i++){
        timeWheel[i] = (timeSlot_t*)calloc(1,sizeof(timeSlot_t));
        timeWheel[i]->head = NULL;
        timeWheel[i]->tail = NULL;
    }

    // epollAdd(epfd,exitPipe[0]);
    char buf[4096]={0};

    // usernames 数组，netfd 作为下标存储用户名
    // 声明并置空 usernames 数组
    char *usernames[1024];
    for(int i = 0; i < 1024; i++){
        usernames[i] = NULL;
    }
    pathstack_t stack;
    // curr_time 用于存储当前时间
    // last_time 为上一秒的时间
    time_t curr_time = time(NULL);
    time_t last_time = curr_time;
    train_t train;
    while(1){
        // 1. 监听 epoll 就绪集合，设置超时时间为 1 秒
        // 2. 遍历就绪集合，判断是 sockfd 还是 netfd 就绪
        struct epoll_event readySet[1024];
        int readyNum = epoll_wait(epfd,readySet,1024, 500);
        // printf("1\n");
        for(int i = 0; i < readyNum; ++i){
            if(readySet[i].data.fd == sockfd){
                // 如果是 sockfd 则 accept 并接收 0 号指令
                int netfd = accept(sockfd,NULL,NULL);
                printf("netfd = %d;\n",netfd);
                //recv tag
                memset(train.data, 0, sizeof(train.data));
                int ret = recv(netfd,&train.len,sizeof(train.len),0);
                recv(netfd,train.data,train.len,0);
                if(ret == 0){
                    continue;
                }
                printf("id = %s;\n",train.data);
                if(strcmp(train.data,"0")==0){
                    // 接收到的 0 号指令如果是 0 则是登录或注册
                    // 1. first_cmd 判断登录或注册
                    // 2. 接收指令，0 表示 signup 注册，1 表示 login 登录
                    // 3. 将时间轮当前轮槽头插 netfd
                    // 4. 登录或注册通过后将 netfd 纳入监听
                    // 5. 以 netfd 为下标存储用户名 
                    // 6. 创建令牌 token 并将 token 传回客户端
                    int first_cmd;
                    int ret = recv(netfd, &first_cmd, sizeof(first_cmd), MSG_WAITALL);
                    if(ret == 0){
                        continue;
                    }
                    printf("f_cmd = %d;\n",first_cmd);
                    if(first_cmd == 0){
                        cmd_signup_user(mysql,netfd,&usernames[netfd]);
                    } else {
                        cmd_login_user(mysql, netfd, &usernames[netfd]);
                    }
                    headInsert(timeWheel[timeWheelIdx], netfd);
                    //调用token生成函数
                    epollAdd(epfd,netfd);
                    char *jwt;
                    create_token(usernames[netfd], &jwt);
                    train.len=strlen(jwt);
                    strcpy(train.data,jwt);
                    send(netfd, &train, sizeof(train.len)+train.len,0);
                }
                if(strcmp(train.data,"1")==0){
                    // 接收到的 0 号指令如果是 1 则是 puts gets 指令
                    // 1. 更新时间轮
                    // 2. 分配任务给子线程
                    memset(train.data, 0, sizeof(train.data));
                    ret = recv(netfd,&train.len,sizeof(train.len),0);
                    if(ret == 0){
                        break;
                    }
                    ret = recv(netfd,train.data,train.len,0);
                    if(ret == 0){
                        break;
                    }
                    char *user = (char *)calloc(1, strlen(train.data) + 1);
                    strcpy(user, train.data);
                    printf("user=%s\n",train.data);
                    int netfd_m;
                    for(int k = 0; k < 1024; k++){
                        if(usernames[k]!=NULL && strcmp(user, usernames[k]) == 0){
                            netfd_m = k;
                            break;
                        }
                    }
                    printf("netfd_m = %d\n",netfd_m);
                    printf("一位用户连接！\n");
                    // 更新时间轮
                    // 1. 先遍历找到当前 netfd 的位置，并将其删除
                    // 2. 在时间轮当前轮槽头插
                    for(int i = 0; i < WHEELSIZE; i++){
                        printf("i = %d;\n", i);
                        int ret = deltimeNode(timeWheel[i], netfd_m);
                        // 返回值是 0 表示成功删除结点
                        // 直接退出循环
                        if (ret == 0){
                            break;
                        }
                    }
                    headInsert(timeWheel[timeWheelIdx], netfd_m);

                    // 分配任务
                    // 进入 worker 进程
                     printf("###@@@@\n");
                    pthread_mutex_lock(&threadPool.mutex);
                    enQueue(&threadPool.taskQueue,netfd);
                    pthread_cond_signal(&threadPool.cond);
                    pthread_mutex_unlock(&threadPool.mutex);
                }    
            }
            else{
                // 如果是 netfd 就绪
                int netfd = readySet[i].data.fd;
                train_t train_user, train_token;
                // 接收客户端的用户名
                memset(train_user.data,0,sizeof(train_user.data));
                int ret = recv(netfd,&train_user.len,sizeof(train.len),0);
                if(ret == 0){
                    break;
                }
                ret = recv(netfd,train_user.data,train_user.len,0);
                if(ret == 0){
                    break;
                }
                printf("user = %s;\n",train_user.data);
                train_user.data[train_user.len] = '\0';


                // 更新时间轮
                // 1. 先遍历找到当前 netfd 的位置，并将其删除
                // 2. 在时间轮当前轮槽头插
                for(int k = 0; k < WHEELSIZE; k++){
                    int ret = deltimeNode(timeWheel[k], netfd);
                    // 返回值是 0 表示成功删除结点
                    // 直接退出循环
                    if (ret == 0){
                        break;
                    }
                }
                headInsert(timeWheel[timeWheelIdx], netfd);

                memset(train_token.data,0,sizeof(train_token.data));
                recv(netfd, &train_token.len, sizeof(train.len),0);
                recv(netfd, train_token.data, train_token.len,0);
                int cret;
                check_token(train_user.data, train_token.data, &cret);
                if(cret == 0){
                    //token验证通过，发送"0"表示验证通过
                    bzero(train.data,sizeof(train.data));
                    train.len=1;
                    train.data[0]='0';
                    send(netfd,&train,sizeof(train.len)+train.len,0);
                    memset(train.data,0,sizeof(train.data));
                    int ret=recv(netfd,&train.len,sizeof(train.len),0);
                    //如果返回值是0的话代表客户端退出，那么退出循环
                    if(ret==0){
                        close(netfd);
                        printf("one client exit!\n");
                    }   
                    recv(netfd,train.data,train.len,0);
                    stack_init(&stack, train_user.data);
                    char cc[100];
                    memset(cc,0,sizeof(cc));
                    strcpy(cc,train.data);
                    char* ccc=strtok(cc," ");
                    ccc=strtok(NULL," ");

                    char str[100];
                    strncpy(str, train.data, 100);
                    char *filename;
                    char *cmd=strtok(train.data," ");
                    filename_get(str,&filename);
                    char *pathname=virtual_path_get(&stack);
                    if(strcmp(cmd,"cd")==0){
                        cmd_cd(netfd,&stack,mysql,ccc,train_user.data);
                    }
                    else if(strcmp(cmd,"ls")==0){
                        cmd_ls(netfd,&stack,mysql,train_user.data);
                    }
                    else if(strcmp(cmd,"pwd")==0){
                        cmd_pwd(netfd,&stack);
                    }
                    else if(strcmp(cmd,"rm")==0){
                        int ret=cmd_rm(mysql,netfd,filename,train_user.data,pathname);
                        LOG(train_user.data,cmd,ret);
                    }
                    else if(strcmp(cmd,"rmdir")==0){
                        int ret=cmd_rmdir(mysql,netfd,filename,train_user.data,pathname);
                        LOG(train_user.data,cmd,ret);
                    }
                    else if(strcmp(cmd,"mkdir")==0){
                        int ret=cmd_mkdir(mysql,netfd,filename,train_user.data,pathname);
                        LOG(train_user.data,cmd,ret);
                    }
                    else if(strcmp(cmd,"userdel")==0){
                        int ret=cmd_userdel_s(netfd,train_user.data,mysql);
                        LOG(train_user.data,cmd,ret);
                    }
                }
                else{
                    bzero(train.data,sizeof(train.data));
                    //验证未通过不执行命令
                    train.len=1;
                    train.data[0]='1';
                    send(netfd,&train,sizeof(train.len)+train.len,0);
                }

            }

        }
        // 检查时间轮
        curr_time = time(NULL);
        if (curr_time - last_time >= 1){
            timeWheelIdx = (timeWheelIdx + 1) % WHEELSIZE;
            printf("timeWheelIdx = %d\n", timeWheelIdx);
            while (timeWheel[timeWheelIdx]->head != NULL){
                printf("netfd = %d\n", timeWheel[timeWheelIdx]->head->netfd);
                close(timeWheel[timeWheelIdx]->head->netfd);
                // 链表头删
                headDel(timeWheel[timeWheelIdx]);
            }
            last_time = curr_time;
        }
    }
    return 0;
}
