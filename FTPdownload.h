#ifndef FTPdownload_h
#define FTPdownload_h

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <stdio.h>


#define MAX_IP_LEN 16
#define SERVER_PORT 21


/*function to get the ip of the host*/
char* get_server_ip();
int server_connection(int port, int receivesResponse);

#endif /*FTPdownload_h*/
