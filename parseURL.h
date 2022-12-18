#ifndef parseURL_h
#define parseURL_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_STR_LEN 254

typedef struct{
char* user;
char* password;
char* host;
char* filepath;
char* ip;
int data_port;
long int data_sockfd;
}connection_info;

/*function to parse the given URL by the user*/
connection_info* parseURL (char* url);
int calculateNewServerPort(char* responseToPasv);


#endif /*parseURL_h*/
