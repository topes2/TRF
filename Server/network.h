#ifndef network_H
#define network_H

typedef struct {
    int socket;
    char userName[10];
} client;

//Funções relacionadas com a rede (server side)
void NetworkS(client *clients, int maxClients);
int Socket();
struct sockaddr_in ServerConfig(struct sockaddr_in server_addr, int port);
void Bind(int serverSocket, struct sockaddr_in server_addr);
void Listen(int serverSocket, int maxClients);

#endif