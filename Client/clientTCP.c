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

    int loggedin = 0; //0 - false, 1 - true

    //Functions to connect to the server
    sockfd = Socket();
    server = ServerbyName(SERVER_BY_NAME);
    serv_addr = ServerSetup(serv_addr, PORT, server);
    Connect(sockfd, serv_addr);

    readFromServer(sockfd, buffer); //handles the first "Wellcome" message from the server
    //loop
    while(1){
        //Interaction
        getInput(buffer); //get the user input and return the size of it, save it on a buffer
        
        if(!loggedin){  //checks if its already logged in
            char *loginCommand = formatingLogin(buffer);         
           if(loginCommand == NULL){ //check if the command given was login or not
                printf("Invalid login command\n");
            } else {
                sends(sockfd, loginCommand); // send the size to check with the max allowed size of transfers
                printf("loggind commandsa: %s\n", loginCommand);
                loggedin = login(sockfd, buffer, loginCommand); //sends the login command to the server
                printf("login\n");
            }

        } else { 
            printf("chegou \n");
            
            //its already logged in
            char *res = formating(buffer);
            if (res != NULL){
                if(!strncmp(ASK_CODE, res, strlen(ASK_CODE)) || !strncmp(ANSWER_CODE, res, strlen(ANSWER_CODE))){//these commands all share a function so they are grouped up
                    QandA(sockfd, buffer, res);
                }else if(!strncmp(PUTFILES_CODE, res, strlen(PUTFILES_CODE))){// these commands all share functions so they are grouped up





                    //to be made by the one the only the magestic rui
                }else if(!strncmp(LISTQUESTIONS_CODE, res, strlen(LISTQUESTIONS_CODE)) || !strncmp(LISTFILES_CODE, res, strlen(LISTFILES_CODE)) ||!strncmp(GETFILES_CODE, res, strlen(GETFILES_CODE))){ //the size of the message being sent is never going to be more than 25
                    write(sockfd,res,strlen(res));
                }

            } else {
                //printf("Invalid Command\n");
                sends(sockfd,buffer);
                writear(sockfd, buffer);
            }
        } 
    }

    close(sockfd);
    return 0;
}