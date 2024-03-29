#ifndef network_H
#define network_H

#include "../../configs.h"

typedef struct{
    int socket;
    char userName[MAX_USERNAME_LENGTH];
} client; 

//Functions
//Server
int Socket();
void NetworkS(client *clients, int maxClients);
struct sockaddr_in ServerConfig(struct sockaddr_in server_addr, int port);
void Bind(int serverSocket, struct sockaddr_in server_addr);
void Listen(int serverSocket, int maxClients);
int acceptNewConnection(int serverSocket, client *clients, struct sockaddr_in *client_addr, socklen_t *addr_len);

//Select
int prepareSelectServer(int serverSocket, client *clients, fd_set *readfds, int *maxSocket);

//Sending and receiving 1024 at a time
int readar(int socket, char* buffer,int size);
void writear(int socket,char* buffer);
void sends(int socket,char* buffer); // send size
int recs(int socket); //receive size
#endif