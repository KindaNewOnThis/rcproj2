#ifndef parse.h
#define parse.h

#include <stdio.h>

#define MAX_STR_LEN 256

typedef struct{
char* user;
char* password;
char* host;
char* filepath;
char* ip;
}connection_info

/*function to parse the given URL by the user*/
connection_info parseURL (char* url);


#endif parse.h
