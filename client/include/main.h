#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <mysql/mysql.h>
#include <openssl/sha.h> 

#include "cmd_dir.h"
#include "cmd_send.h"
#include "cmd_login.h"
#include "cmd_trans.h"
#include "sup_hash.h"
#include "client_ready.h"
#include "cmd_rm.h"

#define ARGS_CHECK(x,y) {if(x!=y){fprintf(stderr,"args错误!\n");}}
#define ERROR_CHECK(x,y,z) {if(x!=y){ perror(z);return -1;}}
#define THREAD_ERROR_CHECK(x,y) {if(x!=0){fprintf(stderr,"%s:%s",y,strerror(x));}}

//用户注销函数
int cmd_userdel_c(int netfd);
//客户端命令检测
int com_judg(char* command);
//客户端tcp建立连接
int tcp_init(char* ip,char* port,int* fd);
//添加目标监听fd到epfd中
int epoll_add(int epfd,int fd);
//将fd从监听集合epfd中移除
int epoll_del(int epfd,int fd);
void * threadFunc(void *arg);

typedef struct train_s{
    long len;
    char data[4096];
}train_t;
