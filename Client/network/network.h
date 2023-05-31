#ifndef CLI_NETWORK_H
#define CLI_NETWORK_H

//Functions
//Client
int Socket();
struct hostent* ServerbyName(char name[]);
struct sockaddr_in ServerSetup(struct sockaddr_in serv_addr, int port, struct hostent *server);
int readFromServer(int sockfd, char *buffer);
void Connect(int sockfd, struct sockaddr_in serv_addr);

#endif