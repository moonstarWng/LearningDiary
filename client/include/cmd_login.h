#ifndef __LOGIN__
#define  __LOGIN__

int cmd_signup_user(int sockfd, char* user, char* password);
int cmd_login_user(int sockfd, char* user, char* password);

#endif
