#ifndef __TOKEN__
#define __TOKEN__

int create_token(char *user, char**jwt);
int check_token(char *user, char *JWT, int *pvret);


#endif
