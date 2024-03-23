#include "../include/main.h"

int mysql_link(MYSQL **pmysql)
{
    *pmysql=mysql_init(NULL);//连接初始化
    char *host = "localhost";//默认为本机
    char *user = "root";//默认为root用户
    char *password = "991220";//本机mysql的登录密码,测试请修改为自己mysql密码
    char *database = "netdisk";//默认存在数据库netdisk，测试之前请先执行create database netdisk;创建数据库
    MYSQL *ret = mysql_real_connect(*pmysql,host,user,password,database,0,NULL,0);
    if(ret == NULL)//判断连接结果
    {
        printf("Error: %s\n", mysql_error(*pmysql));
        return 1;
    }
    return 0;
}
