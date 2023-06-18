#ifndef CLI_NETWORK_H
#define CLI_NETWORK_H

//Functions
//Client
int Socket();
struct hostent* ServerbyName(char name[]);
struct sockaddr_in ServerSetup(struct sockaddr_in serv_addr, int port, struct hostent *server);
void Connect(int sockfd, struct sockaddr_in serv_addr);

int readar(int socket, char* buffer,int size);
void writear(int socket,char* message);

#endif