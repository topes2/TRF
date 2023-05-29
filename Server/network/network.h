#ifndef network_H
#define network_H

typedef struct{
    int socket;
    char userName[10];
    volatile int *flag; //flag que termina o programa
    GDBM_FILE dbLogin;
    fd_set *readfds;
} clientData; 

//Functions
//Server
int Socket();
void NetworkS(clientData *clients, int maxClients);
struct sockaddr_in ServerConfig(struct sockaddr_in server_addr, int port);
void Bind(int serverSocket, struct sockaddr_in server_addr);
void Listen(int serverSocket, int maxClients);
int acceptNewConnection(int serverSocket, clientData *clients, struct sockaddr_in *client_addr, socklen_t *addr_len);
void handleNewConnection(int serverSocket, fd_set *readfds, GDBM_FILE db, struct sockaddr_in *client_addr, socklen_t *addr_len, clientData *clients, volatile int *flag);
    //Select
int prepareSelectServer(int serverSocket, clientData *clients, fd_set *readfds, int *maxSocket);

#endif