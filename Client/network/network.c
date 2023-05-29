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

int readFromServer(int sockfd, fd_set *readfds, char *buffer, int loggedin, int firstmsg) {
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t len = read(sockfd, buffer, sizeof(buffer));
    //if the server disconects
    if (len <= 0) {
        printf("Server disconnected...\n");
        return -1;   
    }

    //Server responses
    if(loggedin || firstmsg){
        printf("%s", buffer);
        firstmsg = 0;
    }
    
    return 0;
}


//Select
int prepareSelect(int sockfd, fd_set *readfds, int *max_fd){
    FD_ZERO(readfds);
    FD_SET(sockfd, readfds);
    FD_SET(STDIN_FILENO, readfds);
    *max_fd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;

    int activity = select(*max_fd + 1, readfds, NULL, NULL, NULL);
    if (activity < 0) {
        perror("Error in select");
        return -1;
    }
    return activity;
}