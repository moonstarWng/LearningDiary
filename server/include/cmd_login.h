#ifndef __LOGIN__
#define __LOGIN__

int cmd_signup_user(MYSQL* mysql, int netfd, char **user);
int cmd_login_user(MYSQL* mysql, int netfd, char **user);

#endif
