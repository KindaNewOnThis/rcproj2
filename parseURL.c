#include "parseURL.h"
#include <stdbool.h>

static connection_info info;

bool hasUsername = false;


connection_info* parseURL (char* url){

    info.user = (char*) malloc(MAX_STR_LEN);
    memset(info.user, 0, MAX_STR_LEN);
    info.password = (char*) malloc(MAX_STR_LEN);
    memset(info.password, 0, MAX_STR_LEN);
    info.host = (char*) malloc(MAX_STR_LEN);
    memset(info.host, 0, MAX_STR_LEN);
    info.filepath = (char*) malloc(MAX_STR_LEN);
    memset(info.filepath, 0, MAX_STR_LEN);
    
    for (int i = 6; i < strlen(url); i++){
        if (url[i] == ':'){
            hasUsername = true;
        }
    }
    
    if (hasUsername){
	    int i = 6;
	    int element_pos = 0;
	    int state=0;
	    int url_length = strlen(url);
	    while(i < url_length){
	    
		switch(state){
		
		    case 0:
		    
		        if(url[i] == ':') {
		            element_pos = 0;
		            state = 1;
		            
		        } 
		        else {
		            info.user[element_pos] = url[i];
		            element_pos++;
		        }
		        break;
		        
		    case 1:
		    
		        if(url[i] == '@'){
		            element_pos = 0;
		            state = 2;
		            
		        } 
		        else {
		            info.password[element_pos] = url[i];
		            element_pos++;
		        }
		        break;
		        
		    case 2:
		    
		        if(url[i] == '/'){
		            element_pos = 0;
		            state = 3;
		        } 
		        else {
		            info.host[element_pos] = url[i];
		            element_pos++;
		        }
		        break;
		        
		    case 3:
		    
		        info.filepath[element_pos] = url[i];
		        element_pos++;
		        break;
		}
		
		i++;
		
	    }
    }
    else{
        int i = 6;
    	int element_pos = 0;
    	int state=0;
    	int url_length = strlen(url);
        printf("i=%d", i);
        info.user = "anonymous";
        
        while(i < url_length){
	    
		switch(state){		
		        
		    case 0:
		    
		        if(url[i] == '/'){
		            element_pos = 0;
		            state = 1;
		        } 
		        else {
		            info.host[element_pos] = url[i];
		            element_pos++;
		        }
		        break;
		        
		    case 1:
		    
		        info.filepath[element_pos] = url[i];
		        element_pos++;
		        break;
		}
		
		i++;
		
	    }
    }
    
    
    return &info;

}

int calculateNewServerPort(char* responseToPasv){

    char pasvCodes[25];
    int i = 0;
    while (responseToPasv[i] != '('){
        i++;
    }
   
    
    for(int j = i, k = 0; j < strlen(responseToPasv); j++,k++){
        pasvCodes[k] = responseToPasv[j];
    }  
    
    int ip, numPort1, numPort2;
    
    sscanf(pasvCodes, "(%d,%d,%d,%d,%d,%d)", &ip,&ip,&ip,&ip,&numPort1,&numPort2);
    info.data_port = (numPort1 * 256 + numPort2);
    return info.data_port;

}
