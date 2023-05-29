#ifndef network_H
#define network_H

//Functions
//Client
int Socket();
struct hostent* ServerbyName(char name[]);
struct sockaddr_in ServerSetup(struct sockaddr_in serv_addr, int port, struct hostent *server);
int readFromServer(int sockfd, fd_set *readfds, char *buffer, int loggedin, int firstmsg);
void Connect(int sockfd, struct sockaddr_in serv_addr);
    //Select
int prepareSelect(int sockfd, fd_set *readfds, int *max_fd);

#endif