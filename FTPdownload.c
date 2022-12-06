#include "parseURL.h"
#include "FTPdownload.h"

#include <stdio.h>

static connection_info* info;


char* readServerResponse (int sockfd){
    
    char buf;
    int bytes = 0;
    char* responseCode = (char*) malloc(MAX_STR_LEN);
    memset(responseCode, 0, MAX_STR_LEN);
    int state;
    int i = 0;
    
    /*printf("BBBBBBBBBBB\n");*/
    
    /*while (buf != '\n'){ é melhor ler assim ou ler carater a carater até ser newline?*/
    	/*printf("BBBBBBBBBBB\n");*/
        bytes += read (sockfd, responseCode, MAX_STR_LEN);
        /*printf("CCCCCCCCCCC\n");*/
        /*printf("digit received = %c\n", buf);*/
        /*responseCode[i] = buf;*/
        /*printf("intermidiate response code = %s\n", responseCode);*/
        i++;
    //}
    
    /*printf("response code = %s\n", responseCode);*/
    return responseCode;
}

void sendMessageToServer(int sockfd, char* message, char* parameter){

    char* messageToSend = (char*) malloc(MAX_STR_LEN);
    memset(messageToSend, 0, MAX_STR_LEN);
    
    strcat(messageToSend, message);
    
    if(parameter != NULL){
        strcat(messageToSend, parameter);
    }
    
    strcat(messageToSend, "\r\n");
    write(sockfd, messageToSend, strlen(messageToSend));
    return;    
}

char* getFileName(){
    
    char* filename = (char*) malloc(MAX_STR_LEN);
    memset(filename, 0, MAX_STR_LEN);
    
    int i = 0;
    int j = 0;
    
    while (i < strlen(info->filepath)){
    
        if (info->filepath[i] != '/'){
            filename[j] = info->filepath[i];
        }
        else{
            memset(filename, 0, MAX_STR_LEN);
            j = 0;
            i++;
            continue;
        }
        
        i++;
        j++;
    }
    
    return filename;
    
}

int getFile(char* filename){

    int bytesRead = 0;
    int totalBytesRead = 0;
    char* message = (char*) malloc(MAX_STR_LEN);
    memset(message, 0, MAX_STR_LEN);
    FILE* filefd = fopen(filename, "w");
    
    if (filefd == NULL){
        printf("Error opening file\n");
        exit(1);
    }
    
    printf("Reading file...\n");
    
    while (bytesRead = read(info->data_sockfd, message, 1) > 0){
    
    	totalBytesRead += bytesRead;    
        fseek(filefd, 0, SEEK_END);
        fwrite(message, sizeof(unsigned char), bytesRead, filefd);
    }
    
    fclose(filefd);
    return totalBytesRead;
}

int sendRetrCommand(int sockfd){
    printf("Retr command is going to be sent...\n");
    printf("info->filepath = %s\n", info->filepath);
    sendMessageToServer(sockfd, "retr ", info->filepath);
    
    char* serverResponseCode = (char*) malloc(MAX_STR_LEN);
    memset(serverResponseCode, 0, MAX_STR_LEN);
    serverResponseCode = readServerResponse(sockfd);
    printf("server response code = %s", serverResponseCode);
   

    while (serverResponseCode[0] == '1'){
        char* serverResponseCode = (char*) malloc(MAX_STR_LEN);
    	memset(serverResponseCode, 0, MAX_STR_LEN);
    	serverResponseCode = readServerResponse(sockfd);
    	printf("server response code = %s", serverResponseCode);
    	
    	if (serverResponseCode[0] == '2'){
    	    break;
        }
    }
    
    char* filename = (char*) malloc(MAX_STR_LEN);
    filename = getFileName();
    printf("filename: %s\n", filename);
    
    int totalBytesRead;
    totalBytesRead = getFile(filename);
    
    close(info->data_sockfd);
    	
    return totalBytesRead;
}

int sendPasvCommand(int sockfd){
    printf("Pasv command is going to be sent...\n");
    sendMessageToServer(sockfd, "pasv", NULL);
    
    char* serverResponseCode = (char*) malloc(MAX_STR_LEN);
    memset(serverResponseCode, 0, MAX_STR_LEN);
    serverResponseCode = readServerResponse(sockfd);
    printf("server response code = %s\n", serverResponseCode);
    
    long int newServerPort;
    newServerPort = calculateNewServerPort(serverResponseCode);
    
    
    if (serverResponseCode[0] != '2'){
    	/*printf("DAKDAKPDKAPDKAWPDKWA\n");*/
    	return 1;
    }
    else{
        return newServerPort;
    }
}

int loginServer(int sockfd){

    printf("Username is going to be sent...\n");
    sendMessageToServer(sockfd, "user ", info->user);
    
    char* serverResponseCode = (char*) malloc(MAX_STR_LEN);
    memset(serverResponseCode, 0, MAX_STR_LEN); 
    
    while (serverResponseCode[0] != '3'){
    	serverResponseCode = readServerResponse(sockfd);
    	printf("server response code = %s\n", serverResponseCode);
    }
    
    printf("Password is going to be sent...\n");
    sendMessageToServer(sockfd, "pass ", info->password);
    
    char* serverResponseCode_ = (char*) malloc(MAX_STR_LEN);
    memset(serverResponseCode_, 0, MAX_STR_LEN);
    serverResponseCode_ = readServerResponse(sockfd);
    printf("server response code = %s\n", serverResponseCode_);
       
    if(serverResponseCode_[0] != '2'){
        return 1;
    }
    else{
        return 0;
    }
}


char* get_server_ip(){
    struct hostent *h;
    char* ip = (char*) malloc (MAX_IP_LEN);
    memset(ip, 0, MAX_IP_LEN);
    if ((h = gethostbyname(info->host)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }
    ip = inet_ntoa(*((struct in_addr *)h->h_addr));
    return ip;
}

int server_connection(int port, int receivesResponse){
    printf("Data socket is going to be opened\n");
    int sockfd;
    struct sockaddr_in server_addr;
    char buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";
    size_t bytes;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(info->ip);
    server_addr.sin_port = htons(port);

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    /*connect to the server*/
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }
    
    /*send a string to the server
    bytes = write(sockfd, buf, strlen(buf));
    
    if (bytes > 0){
        printf("Bytes escritos %ld\n", bytes);
    }
        
    else {
        perror("write()");
        exit(-1);
    }

    if (close(sockfd) < 0) {
        perror("close()");
        exit(-1);
    }*/
    
    
    if (receivesResponse){
        char* serverResponseCode = (char*) malloc(MAX_STR_LEN);
        memset(serverResponseCode, 0, MAX_STR_LEN);
    
        printf("sockfd = %d\n", sockfd);
    
        while(serverResponseCode[0] != '2'){
            serverResponseCode = readServerResponse(sockfd);
        }
    
        printf("server response: %s\n", serverResponseCode);
    }
    
    return sockfd;
}


int main(int argc, char** argv){
    if((info = parseURL(argv[1])) == NULL){
    	printf("sum aint right");
    }
    else{
    	printf("issall good\n");
        printf("username = %s\n", info->user);
    	printf("password = %s\n", info->password);
    	printf("host = %s\n", info->host);
    	printf("filepath = %s\n", info->filepath);
    }
    info->ip = get_server_ip();
    printf("server ip = %s\n", info->ip);
    
    int sockfd;
    
    sockfd = server_connection(SERVER_PORT, 1);
    printf("sockfd = %d\n", sockfd);
    
    if (loginServer(sockfd) != 0){
        close(sockfd);
        exit(1);
    }
    
    long int newServerPort;  
    newServerPort = sendPasvCommand(sockfd);
    
    if (newServerPort == 1){
        close(sockfd);
        exit(1);
    }
    
    printf("new server port = %d\n", info->data_port);
    
    
    info->data_sockfd = server_connection(info->data_port, 0);
    printf("data_sockfd = %ld\n", info->data_sockfd);
    
    int totalBytesRead;
    totalBytesRead = sendRetrCommand(sockfd);
    
    printf("A file of %d bytes was successfully downloaded\n", totalBytesRead);
    
    close(sockfd);
    return 0;
              
}
