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

//server side
//set defeult value of 0 to clients sockets array
void NetworkS(client *clients, int maxClients){
    for (int i = 0; i < maxClients; i++) {
        clients[i].socket = 0;
    }
}

struct sockaddr_in ServerConfig(struct sockaddr_in server_addr, int port){
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    return server_addr;
}

void Bind(int serverSocket, struct sockaddr_in server_addr){
    if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding\n");
        exit(-1);
    }
}

void Listen(int serverSocket, int maxClients){
    if (listen(serverSocket, maxClients) < 0) {
        perror("Error listening\n");
        exit(-1);
    }
}

int prepareSelectServer(int serverSocket, client *clients, fd_set *readfds, int *maxSocket) {
    FD_ZERO(readfds);
    *maxSocket = serverSocket;
    FD_SET(serverSocket, readfds);

    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].socket > 0){
            FD_SET(clients[i].socket, readfds);
        }
        //definir a maior socket
        if(*maxSocket < clients[i].socket){
            *maxSocket = clients[i].socket;
        }
    }

    int activity = select(*maxSocket + 1, readfds, NULL, NULL, NULL);
    if(activity < 0){
        printf("Error in Select\n");
        return -1;
    }
    return activity;
}

int acceptNewConnection(int serverSocket, client *clients, struct sockaddr_in *client_addr, socklen_t *addr_len) {
    int newSocket = accept(serverSocket, (struct sockaddr *)client_addr, addr_len);
    if(newSocket < 0){
        perror("Error accepting connection\n");
        return -1;
    } 

    printf("New client connected!\n");
    write(newSocket, WELCOME_MSG, strlen(WELCOME_MSG));

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == 0) {
            clients[i].socket = newSocket;
            break;
        }
    }
    return 0;
}


char* readar(int socket, char* buffer){
    int nBytes;
    char *message = malloc(MAX_MESSAGE_LENGTH);

    memset(message, 0, MAX_MESSAGE_LENGTH);
    read(socket, message, MAX_MESSAGE_LENGTH);


    if(sscanf(message, "%d", &nBytes) != 1){
        return NULL;
    }

    memset(buffer, 0, BUFFER_SIZE);

    if(nBytes <= MAX_MESSAGE_LENGTH){
        printf("< 1024\n");
        read(socket, buffer, MAX_MESSAGE_LENGTH);
    } else {
        char *pt = buffer;

        while(nBytes > 0 ){
            memset(message, 0, MAX_MESSAGE_LENGTH);
            read(socket, message, MAX_MESSAGE_LENGTH);
            strncpy(buffer, message, strlen(message));
            nBytes -= MAX_MESSAGE_LENGTH;
            pt += MAX_MESSAGE_LENGTH;
        }
    }

    return NULL;
}