#ifndef __PASSCHECK__
#define __PASSCHECK__

#include <stdbool.h>

int cmd_pass_check(MYSQL *msq, char *password, char* user);
int cmd_name_check(MYSQL* mysql, char *username);
#endif

