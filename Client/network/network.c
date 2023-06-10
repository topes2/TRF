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

/*
int writear(int socket,char* buffer){
    int size = strlen(buffer);
    write()

    
}
*/


/*
void writear(int socket, char* buffer){
    printf("Begin write!\n");
    write(socket, READ_CODE, strlen(READ_CODE)); //Anounce to the server

    int nBytes = strlen(buffer);
    char nBytesString[22];

    sprintf(nBytesString, "%d\n", nBytes);
    write(socket, nBytesString, strlen(nBytesString));

    if(nBytes <= MAX_MESSAGE_LENGTH){
        write(socket, buffer, nBytes);
    } else {
        char *pt = buffer;
        char *message = malloc(MAX_MESSAGE_LENGTH); 

        while(nBytes > 0){
            strncpy(message, pt, MAX_MESSAGE_LENGTH);
            write(socket, message, strlen(message));
            memset(message, 0, MAX_MESSAGE_LENGTH);
            pt += MAX_MESSAGE_LENGTH - 1;        
        }
    }

    printf("End write\n");
}
*/
