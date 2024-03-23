#ifndef __TRANS__
#define __TRANS__

#define NOTEXIST -1
// filename
// director
int cmd_puts_in_server(MYSQL* mysql, int netfd,char* pathname, char* filename, char *user);
int cmd_gets_in_server(MYSQL* mysql, int netfd, char* pathname,char *filename, char *user);
#endif
