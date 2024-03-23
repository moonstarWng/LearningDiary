#ifndef CMD_RM_H
#define CMD_RM_H

int cmd_rm(MYSQL*mysql,int netfd,char*filename, char *user, char* pathname);
int cmd_rmdir(MYSQL*mysql,int netfd,char*dirname, char *user,char* pathname);
int cmd_mkdir(MYSQL*mysql,int netfd,char*dirname, char *user,char *pathname);

#endif
