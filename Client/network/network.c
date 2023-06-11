#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>

#include "network.h"
#include "../../configs.h"

//Socket creation
int Socket(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed...\n");
        exit(-1);
    }
    return sockfd;
}

//Client side
struct hostent* ServerbyName(char name[]){
    struct hostent *server = gethostbyname(name);
    if (server == NULL) {
        printf("Server connection error...\n");
        exit(-1);
    }
    return server;
}

struct sockaddr_in ServerSetup(struct sockaddr_in serv_addr, int port, struct hostent *server){
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    return serv_addr;
}

void Connect(int sockfd, struct sockaddr_in serv_addr){
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection error...\n");
        exit(-1);
    }
}


int readFromServer(int sockfd, char *buffer) {
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t len = read(sockfd, buffer, BUFFER_SIZE);

    //if the server disconects
    if (len <= 0) {
        printf("Server disconnected...\n");
        return -1;   
    }

    //Server responses
    printf("%s", buffer);
    return 0;
}

int readar(int socket, char* buffer,int size){
    memset(buffer, 0, BUFFER_SIZE);
    char* pt = buffer;
    printf("gay ffs\n");
    while(strlen(buffer) <= size ){
        printf("weeeee \n");
        read(socket,pt,MAX_MESSAGE_LENGTH);
        pt += MAX_MESSAGE_LENGTH;
    }
    printf("gueee \n");
    return strlen(buffer);    
}


void writear(int socket,char* buffer){//versao 1.0
    int size = strlen(buffer);
    printf("size = %d",size);
    if(size <= MAX_MESSAGE_LENGTH){
        write(socket,buffer,size);
    }else{
        char* pt = buffer;
        char tb[MAX_MESSAGE_LENGTH];
        float parts = (float)size/MAX_MESSAGE_LENGTH;
        while(parts>0){
            printf("whiles %f\n",parts);
            if(strlen(pt) >= MAX_MESSAGE_LENGTH){
            memcpy(tb,pt,MAX_MESSAGE_LENGTH);
            write(socket,tb,MAX_MESSAGE_LENGTH);
            pt = pt+ MAX_MESSAGE_LENGTH;
            }else{
                memcpy(tb,pt,strlen(pt));
                write(socket,tb,strlen(pt));
            }
            parts-=1;
        }
    }
}
