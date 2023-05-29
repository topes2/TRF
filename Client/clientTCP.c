#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>

#include "network/network.h"
#include "../configs.h"
#include "funcs/funcs.h"


int main(){
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int max_fd;


    int loggedin = 0, firstmsg = 1; //0 - false, 1 - true

    //Functions to connect to the server
    sockfd = Socket();
    server = ServerbyName(SERVER_BY_NAME);
    serv_addr = ServerSetup(serv_addr, PORT, server);
    Connect(sockfd, serv_addr);

    //loop
    while(1){
        //Select
        if(prepareSelect(sockfd, &readfds, &max_fd) < 0){
            exit(-1);
        }

        //Interaction with server
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            getInput(buffer); //Client input

            

            if(!loggedin){ //login
                char *loginCommand = formatting(buffer);
                if(loginCommand == NULL){
                    printf("Invalid login command\n");
                } else { //Login
                    int loginV = login(sockfd, buffer, loginCommand);
                }
            } else { //Classroom interactions

            }

        }

        //Prints
        if (FD_ISSET(sockfd, &readfds)) {
            if(readFromServer(sockfd, &readfds, buffer, loggedin, firstmsg) < 0){
                break;
            }
        }        
    }

    close(sockfd);
    return 0;
}