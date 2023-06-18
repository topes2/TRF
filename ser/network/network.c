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
//set default value of 0 to clients sockets array
void NetworkS(client *clients, int maxClients){
    for (int i = 0; i < maxClients; i++) {
        clients[i].socket = 0;
        strcpy(clients[i].userName, "GUEST");
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

void sends(int socket,char* buffer){
    int size = strlen(buffer);
    char sizes[11];
    sprintf(sizes, "%d\n", size);  
    write(socket,sizes,strlen(sizes));
    }



int recs(int socket){ //recieves the size of a message incoming to see how many times to read
    char* buffer = malloc(22);
    char c = 'a';
    int size;
    int reader = 0;
    while(c != '\n'){
        reader += read(socket, &c, 1);
        if(reader == -1){
            perror("read");
        } else if(reader == 0){
            return 0;
        }

        *buffer = c;
        buffer+=1;        
    }
    buffer -= reader;
    size = atoi(buffer);
    return size;
}




int readar(int socket, char* buffer,int size){
    memset(buffer, 0, strlen(buffer));
    char* pt = buffer;
    while(strlen(buffer) < size ){
        read(socket,pt,MAX_MESSAGE_LENGTH);
        pt += MAX_MESSAGE_LENGTH;
    }

    return strlen(buffer);    
}


void writear(int socket,char* buffer){//versao 1.0
    int size = strlen(buffer);
    if(size<MAX_MESSAGE_LENGTH){
        write(socket,buffer,size);
    }else{
        char* pt = buffer;
        char tb[MAX_MESSAGE_LENGTH];
        float parts = (float) size/MAX_MESSAGE_LENGTH;

        printf("parts = %f, size: %d\n",parts, size);
        while(parts>0){
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