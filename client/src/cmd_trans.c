#include "../include/main.h"
// 客户端
int cmd_gets_in_client(int sockfd, char* filename){
    // 客户端中下载文件
    train_t train; // train 用于接收各种信息
    // 1. 判断文件是否存在于服务端
    // 如果客户端接收到服务端发送的报错信息 NOTEXIST 则文件不存在，直接返回
    // 接收到正常信息则文件存在
    int error_flag;
    recv(sockfd, &error_flag, sizeof(error_flag), MSG_WAITALL);
    if(error_flag == NOTEXIST){
        printf("%s文件不存在！\n",filename);
        return 1;
    };
    // 2.创建要下载的文件名，如果已有则直接打开
    // 传递文件偏移量用于断点续传
    int fd = open(filename, O_RDWR|O_CREAT, 0666);
    // statbuf 存储文件状态
    struct stat statbuf;
    fstat(fd, &statbuf);
    // 得到偏移量
    off_t lseek_size = statbuf.st_size;
    // 发送偏移量
    send(sockfd, &lseek_size, sizeof(lseek_size), MSG_NOSIGNAL);
    // 3.接收文件
    // 先接收文件总长度
    // 再根据文件总长度进行接收
    recv(sockfd, &train.len, sizeof(train.len), MSG_WAITALL);
    recv(sockfd, train.data, train.len, MSG_WAITALL);
    off_t filesize;
    memcpy(&filesize, train.data, train.len);
    printf("lseek_size = %ld, filesize = %ld\n",lseek_size, filesize);
    // 如果下载文件小于本地文件大小，则直接返回
    if(filesize <= lseek_size + 1){
        printf("%s传输失败，本地已存在！\n",filename);
        return 1;
    }
    off_t currsize = lseek_size;
    lseek(fd, currsize, SEEK_SET);
    off_t lastsize = lseek_size;
    off_t slice = filesize/50;
    char buf[1000];
    while(currsize < filesize){
        bzero(buf, sizeof(buf));
        ssize_t sret = recv(sockfd, buf, (sizeof(buf) > (unsigned long)(filesize-currsize)) ? filesize-currsize:sizeof(buf), 0);
        write(fd, buf, sret);
        if(currsize - lastsize >= slice){
            printf("%5.2lf%%\r",currsize*100.0/filesize);
            fflush(stdout);
            lastsize = currsize;
        }
        currsize += sret;
        lseek(fd, currsize, SEEK_SET);
    }
    printf("100.00%%\n");
    printf("%s传输完成！\n",filename);
    close(fd);
    return 0;
}
int cmd_puts_in_client(int sockfd, char* filename){
    // 客户端上传文件
    train_t train;
    // 1.判断文件是否存在于客户端
    // 告知客户端文件是否存在
    // 若文件不存在则直接退出
    int fd = open(filename, O_RDWR);
    send(sockfd, &fd, sizeof(int), MSG_NOSIGNAL);
    if(fd == NOTEXIST){
        printf("%s文件不存在！\n",filename);
        return 1;
    }
    // 2.计算文件 hash 值
    // 用于服务端文件名
    // 接收服务端文件是否存在
    // 如果已存在则直接退出（秒传）
    // 如果未存在则传输文件
    char hash[4096] = {0};
    sha256_file(filename, hash);
    // 发送hash值
    train.len = strlen(hash);
    memcpy(train.data, hash, train.len);
    send(sockfd, &train, sizeof(train.len) + train.len, MSG_NOSIGNAL);
    // 判断是否秒传
    char ans[2];
    recv(sockfd,&ans, 1,0);
    ans[2] = '\0';
    if(strcmp("0", ans) == 0)
    {
        // 秒传
        printf("传输成功！\n");
        return 0;
    }
    // 3.文件不存在于服务端，需上传文件
    // 接收偏移量，从偏移量开始发送文件
    off_t lseek_size;
    recv(sockfd, &lseek_size, sizeof(lseek_size), MSG_WAITALL);
    // 通过文件描述符获取文件状态
    struct stat statbuf;
    fstat(fd, &statbuf);
    // 发送文件总长度
    off_t filesize = statbuf.st_size;
    train.len = sizeof(off_t);
    memcpy(train.data, &filesize, train.len);
    send(sockfd, &train, sizeof(train.len) + train.len, MSG_NOSIGNAL);
    // 传文件
    // 大文件用零拷贝技术 
    printf("%s开始传输！\n",filename);
    off_t currsize = lseek_size;
    lseek(fd, currsize, SEEK_SET);
    char buf[1000];
    off_t lastsize = lseek_size;
    off_t slice = filesize/30;
    size_t ret;
    if(filesize > 100*1024*1024){
        printf("零拷贝技术！\n");
        while(currsize < filesize){
            // 每次传输 1MB
            ret =sendfile(sockfd, fd, &lseek_size, 1024*1024);
            // printf("currsize = %ld, filesize = %ld, ret = %ld\n",currsize, filesize, ret);
            if(ret <= 0){
                break;
            }
            currsize += ret;
            printf("\r%5.2lf%%\r",currsize*100.0/filesize);
            fflush(stdout);
        }
        printf("100.00%%\n传输成功！\n");
        close(fd);
        return 0;
    }
    // 小文件直接传输
    while(currsize < filesize){
        bzero(buf, sizeof(buf));
        read(fd, buf, sizeof(buf));
        ssize_t sret = send(sockfd, buf, (sizeof(buf) > (unsigned long)(filesize-currsize)) ? filesize-currsize:sizeof(buf),0);
        read(fd, buf, sret);
        printf("\r%5.2lf%%\r", currsize*100.0/filesize);
        currsize += sret;
        lseek(fd, currsize, SEEK_SET);
    }
    printf("100.00%%\n");
    sleep(20);
    printf("传输成功！\n");
    close(fd);
    return 0;

}

