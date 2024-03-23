#ifndef __TRANS__
#define __TRANS__

#define NOTEXIST -1

int cmd_gets_in_client(int sockfd, char* filename);
int cmd_puts_in_client(int sockfd, char* filename);

#endif
