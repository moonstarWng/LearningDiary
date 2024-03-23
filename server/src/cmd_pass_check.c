#include "../include/main.h"

int cmd_pass_check(MYSQL* mysql, char *password, char* username){
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[225];
    char *salt;
    char *encrypted_value;
    snprintf(query, 225, "SELECT salt, encrypted_password FROM users WHERE username = '%s' and tomb = 0;", username);
    // 执行查询
    int ret = mysql_query(mysql, query);
    if(ret != 0){
        return 1;
    }
    result = mysql_use_result(mysql);
    if(result == NULL){
        return 1;
    }
    row = mysql_fetch_row(result);
    if(row == NULL){
        mysql_free_result(result);
        return 1;
    }
    salt = row[0];
    encrypted_value = row[1];
    // 校验密码
    char *curr_value = crypt(password, salt);
    mysql_free_result(result);
    if(strcmp(curr_value, encrypted_value) == 0)
    {
        return 0;
    }else{
        return 1;
    }
}
int cmd_name_check(MYSQL* mysql, char *username){
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[225];
    snprintf(query, 225, "SELECT * FROM users WHERE username = '%s';", username);
    // 执行查询
    int ret = mysql_query(mysql, query);
    result = mysql_use_result(mysql);
    row = mysql_fetch_row(result);
    if(row == NULL){
        mysql_free_result(result);
        return 0;
    }
    mysql_free_result(result);
    return 1;
}

