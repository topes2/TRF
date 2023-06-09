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

void writear(int socket, char* message){
    printf("Message length: %ld\n", strlen(message));
    int cycles = (strlen(message) / MAX_MESSAGE_LENGTH) + 1;

    printf("ncyles: %d\n", cycles);

    char *nCycles = malloc(sizeof(cycles));
    sprintf(nCycles, "%d", cycles);

    printf("string cycles: %s\n", nCycles);
    
    //write(socket, nCycles, strlen(nCycles));

    char *pt = message;
    char *messageAux = malloc(MAX_MESSAGE_LENGTH);

    if(cycles == 1){
        printf("write normal\n");
        //write(socket, message, strlen(message));
    } else {
        for(int i = 0; i < cycles; i++){
            strncpy(messageAux, pt, MAX_MESSAGE_LENGTH);
            printf("%s", messageAux);
            //write
            memset(messageAux, 0, MAX_MESSAGE_LENGTH);
            pt += MAX_MESSAGE_LENGTH;
        }
    }
}

char* readar(int socket, char* buffer){
    char nCyclesString[10];
    //read(socket, nCyclesString, sizeof(nCycles));
    int nCycles = 1;

    sscanf(nCyclesString, "%d", &nCycles);
    
    printf("nCycles = %d\n", nCycles);

    return NULL;
}