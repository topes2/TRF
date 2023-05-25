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

int Socket(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed...\n");
        exit(-1);
    }

    return sockfd;
}

struct hostent* ServerbyName(char name[]){
    struct hostent *server = gethostbyname(name);

    if (server == NULL) {
        printf("Server connection error...\n");
        exit(-1);
    }

    return server;
}

struct sockaddr_in ServerSetup(struct sockaddr_in serv_addr, int port, struct hostent *server) {
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

  return serv_addr;
}

void Connect(int sockfd, struct sockaddr_in serv_addr){
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection error...\n");
        exit(-1);
    }
}