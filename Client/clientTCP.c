#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include "funcs/funcs.h"
#include "network/network.h"
#include "../configs.h"


int main(){
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int max_fd;

    int doingstuff = 0;
    int loggedin = 0, firstmsg = 1; //0 - false, 1 - true

    //Functions to connect to the server
    sockfd = Socket();
    server = ServerbyName(SERVER_BY_NAME);
    serv_addr = ServerSetup(serv_addr, PORT, server);
    Connect(sockfd, serv_addr);

    readFromServer(sockfd, buffer);
    //loop
    while(1){
        //Interaction
        if(doingstuff != 1){
            getInput(buffer);
        }

        if(!loggedin){ 
            doingstuff = 1;
            char *loginCommand = formating(buffer);
           if(loginCommand == NULL){
                printf("Invalid login command\n");
            } else {
                loggedin = login(sockfd, buffer, loginCommand);
            }
            doingstuff = 0;
        } else { //its already logged in
            printf("Other\n");
        } 
    }

    close(sockfd);
    return 0;
}