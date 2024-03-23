#include "../include/main.h"

int cmd_signup_user(MYSQL *mysql, int netfd, char** user){
    train_t train_user, train_password;
    // 注册
    bzero(train_user.data, sizeof(train_user.data));
    int ret = recv(netfd, &train_user.len, sizeof(train_user.len), 0);
    if(ret == 0){
        return 1;
    }
    ret = recv(netfd, train_user.data, train_user.len, 0);
    if (ret == 0){
        return 0;
    }
    train_user.data[train_user.len - 1] = '\0';
    recv(netfd, &train_password.len, sizeof(train_password.len), 0);
    recv(netfd, train_password.data, train_password.len, 0);
    train_password.data[train_password.len - 1] = '\0';
    *user = (char *)calloc(train_user.len, sizeof(char));
    strcpy(*user, train_user.data);
    ret = cmd_name_check(mysql, train_user.data);
    if(ret == 0){
        // ret == 0 则登陆成功
        char *buf = "0";
        printf("注册成功\n");
        send(netfd, buf, strlen(buf), 0);
    }else{
        char *buf = "1";
        printf("注册失败\n");
        send(netfd, buf, strlen(buf), 0);
        return 1;
    }
    MYSQL_RES *result;
    MYSQL_ROW row;
    srand(time(NULL));
    char salt[21];
    salt[0] = '$';
    salt[1] = '6';
    salt[2] = '$';
    salt[19] = '$';
    salt[20] = '\0';
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for(int i = 3; i <19; i++){
        int index = rand() % (sizeof(charset) - 1);
        salt[i] = charset[index];
    }
    char *encrypted_value = crypt(train_password.data, salt);
    // insert into users values(2, 'changfeng', '3', '4', 0);
    char query[225];
    snprintf(query, 225, "SELECT COUNT(id) FROM users;");
    // 执行查询
    ret = mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    row = mysql_fetch_row(result);
    int id;
    if(row == NULL){
        id = 0;
    }else{
        id = atoi(row[0]) + 1;
    }
    char username[100];
    strcpy(username,train_user.data);
    mysql_free_result(result);
    snprintf(query, 225, "INSERT INTO users VALUES(%d, '%s', '%s','%s',0);"
             , id, username, salt, encrypted_value);
    // 执行查询
    ret = mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    mysql_free_result(result);
    // 插入用户根目录文件表
    snprintf(query, 225, "SELECT COUNT(id) FROM files;");
    ret = mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    row = mysql_fetch_row(result);
    if(row == NULL){
        id = 0;
    }else{
        id = atoi(row[0]) + 1;
    }
    strcpy(username,train_user.data);
    mysql_free_result(result);
    snprintf(query, 225, "INSERT INTO files VALUES(%d, '/', '%s', -1, '/', 'd','-', -1, 0);"
             , id, username);
    // 执行查询
    ret = mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    mysql_free_result(result);
}



int cmd_login_user(MYSQL* mysql, int netfd, char** user){
    train_t train_user, train_password;
    // 登录
    // 接收用户名
    int ret = recv(netfd, &train_user.len, sizeof(train_user.len), 0);
    if(ret == 0){
        return 1;
    }
    ret =  recv(netfd, train_user.data, train_user.len, 0);
    if(ret == 0){
        return 1;
    }
    train_user.data[train_user.len - 1] = '\0';
    printf("user = %s;\n",train_user.data);
    *user = (char *)calloc(train_user.len, sizeof(char));
    strcpy(*user, train_user.data);
    // 接受密码
    ret = recv(netfd, &train_password.len, sizeof(train_password.len), 0);
    recv(netfd, train_password.data, train_password.len, 0);
    if(ret == 0){
        return 1;
    }
    train_password.data[train_password.len - 1] = '\0';
    printf("password = %s\n",train_password.data);
    // 检查用户名和密码
    ret = cmd_pass_check(mysql, train_password.data, train_user.data);
    if(ret == 0){
        // ret == 0 则登陆成功
        char *buf = "0";
        printf("登录成功\n");
        send(netfd, buf, strlen(buf), 0);
    }else{
        char *buf = "1";
        printf("登陆失败\n");
        send(netfd, buf, strlen(buf), 0);
    }
    return ret;
}
