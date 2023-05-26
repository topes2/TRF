#ifndef network_H
#define network_H

//struct para os socket
typedef struct {
    int socket[MAX_CLIENTS];
    
} clients;

//Funções relacionadas com a rede (server side)
void NetworkS(int clientSockets[], int maxClients);
int Socket();
struct sockaddr_in ServerConfig(struct sockaddr_in server_addr, int port);
void Bind(int serverSocket, struct sockaddr_in server_addr);
void Listen(int serverSocket, int maxClients);

#endif