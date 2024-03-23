#include "../include/main.h"

//tcp_init连接，成功返回0，失败返回1
int tcp_init(char* ip,char* port,int* fd)
{
    //获取fd
    *fd=socket(AF_INET,SOCK_STREAM,0);

    //将服务器ip、端口存储
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(atoi(port));
    inet_aton(ip,&addr.sin_addr);
    
    //connect尝试连接
    int ret=connect(*fd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret!=0)
    {
        return 1;
    }
    return 0;
}