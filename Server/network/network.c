#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>
#include <gdbm.h>
#include <pthread.h>

#include "network.h"
#include "../../configs.h"
#include "../funcs/funcs.h"

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
void NetworkS(clientData *clients, int maxClients){
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

int prepareSelectServer(int serverSocket, clientData *clients, fd_set *readfds, int *maxSocket) {
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

int acceptNewConnection(int serverSocket, clientData *clients, struct sockaddr_in *client_addr, socklen_t *addr_len) {
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

//Thread
void *handle_client(void *arg){
    clientData *data = (clientData *)arg;
    int clientSocket = data->socket;
    GDBM_FILE dbLogin = data->dbLogin;
    volatile int *flag = data->flag;
    fd_set *readfds = data->readfds;

    char buffer[BUFFER_SIZE];

    //chat
    printf("start chat!\n");
    while(!*flag){
        
        //Leitura dos clientes
        memset(buffer, 0, strlen(buffer));
        printf("read\n");
        ssize_t len = read(clientSocket, buffer, BUFFER_SIZE);

        printf("client said: %s\n", buffer);

        if(len <= 0){ //Cliente exit
            //clientExit(clients, i);
            printf("Client left");
            close(clientSocket);
            break;
        } else {
            if(!strncmp(LOGIN_CODE, buffer, strlen(LOGIN_CODE))){ //User trying to login
                //clientLogin(clientSocket, buffer, dbLogin);
            } else {
                            
            }

        }   
    }
    printf("Exit chat\n");
}

void handleNewConnection(int serverSocket, fd_set *readfds, GDBM_FILE dbLogin, struct sockaddr_in *client_addr, socklen_t *addr_len, clientData *clients, volatile int *flag){
    if(FD_ISSET(serverSocket, readfds)){
        //New connection
        int newSocket = acceptNewConnection(serverSocket, clients, client_addr, addr_len); 
        if(newSocket < 0){
            return;
        }
        
        //Thread creation for the conection
        pthread_t clientThread;
        clientData *data = malloc(sizeof(clientData));
        data->socket = newSocket;
        data->dbLogin = dbLogin;
        data->flag = flag;
        data->readfds = readfds;

        if(pthread_create(&clientThread, NULL, handle_client, data)){
            printf("Client thread creation error");
            close(newSocket);
        } else {
            pthread_detach(clientThread);//free resourses when done
        }

    } 
}

