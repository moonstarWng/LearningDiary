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
#include <l8w8jwt/encode.h>
#include <l8w8jwt/decode.h>

#include "stack.h"
#include "cmd_dir.h"
#include "cmd_userdel.h"
#include "cmd_rm.h"
#include "cmd_pass_check.h"
#include "cmd_trans.h"
#include "cmd_login.h"
#include "task_queue.h"
#include "thread_pool.h"
#include "time_wheel.h"
#include "token.h"

#define ARGS_CHECK(x,y) {if(x!=y){fprintf(stderr,"args错误！\n");}}
#define ERROR_CHECK(x,y,z) {if(x==y){ perror(z);return -1;}}
#define THREAD_ERROR_CHECK(x,y) {if(x!=0){fprintf(stderr,"%s:%s",y,strerror(x));}}
#define LOG(user,cmd,ret) {write_log(user,cmd,ret,__FILE__,__FUNCTION__,__LINE__);} 

int tidArrInit(tidArr_t * ptidArr, int workerNum);
void *threadFunc(void *arg);
int mysql_link(MYSQL **pmysql);
void write_log(char* user,char* cmd,int ret,char* file,const char* func, int line);
int filename_get(char *str,char **filename);
typedef struct train_s{
    long len;
    char data[4096];
}train_t;
