#include "../include/main.h"
// 服务端
int cmd_gets_in_server(MYSQL* mysql, int netfd, char* pathname,char *filename, char* user){
    // 服务端中传输下载的文件
    train_t train;
    // 1. 判断文件是否存在于服务端
    // 通过文件路径和文件名查找文件，如果查找结果为空，则文件不存在
    // 若文件不存在则发送报错信息 NOTEXIST， 发送完退出
    // 否则发送正常信息
    char query[225];
    MYSQL_RES *result;
    snprintf(query, 225, "SELECT * FROM files WHERE path ='%s' and filename = '%s' and tomb = 0 and user = '%s';"
             , pathname, filename, user);
    // 执行查询
    int ret = mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(result);
    if(row = NULL){
        mysql_free_result(result);
        int ans = NOTEXIST;
        send(netfd, &ans, sizeof(int), MSG_NOSIGNAL);
        return 1;
    }
    mysql_free_result(result);
    int ans = 0;
    send(netfd, &ans, sizeof(int), MSG_NOSIGNAL);
    // 2. 根据文件名和文件路径获得文件哈希值
    // 打开服务端以哈希值命名的真实文件
    // 接收文件偏移量
    snprintf(query, 225, "SELECT sha1sum FROM files WHERE path ='%s' and filename = '%s' and user = '%s';"
             , pathname, filename, user);
    // 执行查询
    ret = mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    row = mysql_fetch_row(result);
    char hashname[150];
    strncpy(hashname, row[0], 150);
    mysql_free_result(result);
    char fullname[100] = "../data";
    // 使用 strcat 函数拼接字符串
    strcat(fullname, "/");
    strcat(fullname, hashname); 
    int fd = open(fullname, O_RDWR);
    // 接收偏移量
    off_t lseek_size;
    recv(netfd, &lseek_size, sizeof(lseek_size), MSG_WAITALL);
    // 通过文件描述符获取文件状态
    struct stat statbuf;
    fstat(fd, &statbuf);
    // 3.发送文件
    // 先发送总长度
    // 再根据总长度进行传输
    train.len = sizeof(off_t);
    memcpy(train.data, &statbuf.st_size, train.len);
    send(netfd, &train, sizeof(train.len) + train.len, MSG_NOSIGNAL);
    // 如果下载文件小于本地文件大小，则直接返回
    if(statbuf.st_size <= lseek_size + 1){
        return 1;
    }
    int filesize = statbuf.st_size;
    if(filesize > 100*1024*1024){
        sendfile(netfd, fd, &lseek_size, filesize - lseek_size + 1);
        return 0;
    }
    off_t currsize = lseek_size;
    lseek(fd, currsize, SEEK_SET);
    char buf[1000];
    while(currsize < filesize){
        bzero(buf, sizeof(buf));
        read(fd, buf, sizeof(buf));
        ssize_t sret = send(netfd, buf, (sizeof(buf) > (unsigned long)(filesize-currsize)) ? filesize-currsize:sizeof(buf),0);
        read(fd, buf, sret);
        currsize += sret;
        lseek(fd, currsize, SEEK_SET);
    }
    close(fd);
    return 0;
}
int cmd_puts_in_server(MYSQL* mysql, int netfd,char* pathname, char* filename, char *user){
    // 服务端中接收上传的文件
    train_t train, train_hash;
    // 1.判断文件是否存在于客户端
    // 被客户端告知文件是否存在
    // 若文件不存在则直接退出
    int error_flag;
    recv(netfd, &error_flag, sizeof(error_flag),MSG_NOSIGNAL);
    if(error_flag == NOTEXIST){
        return 1;
    };
    // 2.接受 hash 值
    // train_hash 用于存储 hash 信息
    // 查表文件是否存在，并告知客户端
    // 如果文件存在则修改表格的 tomb 值为0，并直接退出
    recv(netfd, &train_hash.len, sizeof(train_hash.len), MSG_WAITALL);
    recv(netfd, train_hash.data, train_hash.len, MSG_WAITALL);
    train_hash.data[train_hash.len - 1] = '\0';
    // 判断文件是否存在
    char query[225];
    MYSQL_RES *result;
    char hash[100];
    strcpy(hash, train_hash.data);
    printf("hash = %s\n",hash);
    snprintf(query, 225, "SELECT * FROM files WHERE sha1sum = '%s';", hash);
    // 执行查询
    mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(result);
    // 返回查询结果
    if(row != NULL){
        // 文件重复
        mysql_free_result(result);
        char *ans = "0";
        printf("秒传\n");
        send(netfd, ans, strlen(ans), 0);
        // 查表，user、file、path是否存在
        // 如果存在则将tomb修改为0
        // 如果不存在则插表
        snprintf(query, 225, "SELECT id FROM files WHERE user = '%s'"
                 " and path = '%s' and filename = '%s';"
                 , user, pathname, filename);
        // 执行查询
        mysql_query(mysql, query);
        result = mysql_use_result(mysql);
        row = mysql_fetch_row(result);
        printf("user = %s, pathname = %s, filename = %s\n"
               ,user, pathname, filename);
        if(row == NULL){
            // 表里不存在，插表
            // 先找到 pre_id 和文件大小
            snprintf(query, 225, "SELECT id FROM files WHERE path = '%s'"
                                " and type = 'd' and user = '%s';"
                                , pathname, user);
            // 执行查询
            mysql_query(mysql, query);
            result = mysql_use_result(mysql);
            row = mysql_fetch_row(result);
            int pre_id;
            if(row == NULL){
                pre_id = 1;
            } else{
                pre_id = atoi(row[0]);
            }
            mysql_free_result(result);
            char fullname[110];
            sprintf(fullname, "../data/%s", hash);
            int fd = open(fullname, O_RDWR);
            struct stat statbuf;
            fstat(fd, &statbuf);
            // 得到偏移量
            off_t filesize = statbuf.st_size;
            snprintf(query, 225, "INSERT INTO files "
                     "(filename, user, pre_id, path, type, sha1sum, size, tomb) "
                     "VALUES('%s', '%s', %d, '%s', '-', '%s', %ld, 0);"
                     ,filename, user, pre_id, pathname, hash, filesize);
            // 执行查询
            mysql_query(mysql, query);
            result = mysql_use_result(mysql);
            mysql_free_result(result);
            close(fd);
            return 0;
        }
        // 修改 tomb = 0
        snprintf(query, 225, "UPDATE files SET tomb = 0 WHERE user = '%s' "
                 "and path = '%s' and filename = '%s';"
                 , user, pathname, filename);
        // 执行查询
        mysql_free_result(result);
        mysql_query(mysql, query);
        return 0;
    }
    // 3.文件不存在，需上传文件
    // 先打开或创建文件
    // 给客户端发送文件偏移量
    // 接收客户端上传文件
    // 传输成功后信息加入文件表
    char *ans = "1";
    printf("逐字传输\n");
    send(netfd, ans, strlen(ans), 0);
    mysql_free_result(result);
    // 以 hash 值命名文件
    char fullname[100] = "../data";

    // 使用 strcat 函数拼接字符串
    strcat(fullname, "/");
    strcat(fullname, train_hash.data);

    int fd = open(fullname, O_RDWR|O_CREAT, 0666);
    // statbuf 存储文件状态
    struct stat statbuf;
    fstat(fd, &statbuf);
    // 得到偏移量
    off_t lseek_size = statbuf.st_size;
    // 发送偏移量
    send(netfd, &lseek_size, sizeof(lseek_size), MSG_NOSIGNAL);
    // 接收文件总长度
    recv(netfd, &train.len, sizeof(train.len), MSG_WAITALL);
    recv(netfd, train.data, train.len, MSG_WAITALL);
    off_t filesize;
    memcpy(&filesize, train.data, train.len);
    off_t currsize = lseek_size;
    off_t lastsize = lseek_size;
    off_t slice = filesize/30;
    lseek(fd, currsize, SEEK_SET);
    char buf[1000];
    while(currsize < filesize){
        bzero(buf, sizeof(buf));
        ssize_t sret = recv(netfd, buf, (sizeof(buf) > (unsigned long)(filesize-currsize)) ? filesize-currsize:sizeof(buf), 0);
        write(fd, buf, sret);
        currsize += sret;
        lseek(fd, currsize, SEEK_SET);
    }
    // 插入文件表格
    snprintf(query, 225, "SELECT COUNT(id) FROM files;");
    // 执行查询
    int ret = mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    row = mysql_fetch_row(result);
    int id = atoi(row[0]) + 1;
    mysql_free_result(result);
    snprintf(query, 225, "SELECT id FROM files WHERE path = '%s'"
             " and type = 'd' and user = '%s';"
             , pathname, user);
    // 执行查询
    ret = mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    row = mysql_fetch_row(result);
    int pre_id;
    if(row == NULL){
        pre_id = 1;
    } else{
        pre_id = atoi(row[0]);
    }
    mysql_free_result(result);

    snprintf(query, 225, "INSERT INTO files VALUES(%d, '%s', '%s', %d, '%s', '-', '%s', %ld, 0);"
             ,id, filename, user, pre_id, pathname, hash, filesize);
    // 执行查询
    mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    mysql_free_result(result);
    sleep(20);
    printf("传输完成!\n");
    close(fd);
    return 0;

}

