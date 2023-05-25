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
#include "funcs.h"

#define PORT 5555
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    int loggedin = 0; //0 nao fez log in, 1 fez

    sockfd = Socket();
    server = ServerbyName("localhost");
    serv_addr = ServerSetup(serv_addr, PORT, server);

    Connect(sockfd, serv_addr);

    fd_set readfds;
    int max_fd;

    while (1) {
        //select
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        max_fd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;

        
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Error in select\n");
            exit(EXIT_FAILURE);
        }

        //Chat
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            //leitura do terminal
            bzero(buffer, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);

            //comando de saida
            if (strcmp("exit\n", buffer) == 0) {
                printf("Leaving...\n");
                break;
            }

            if(!loggedin){
                char* userPass = formatting(buffer);
                if(userPass == NULL){
                    printf("Invalid login command\n");
                } else{
                    write(sockfd, userPass, strlen(userPass));
                }
                loggedin = 1;
            } else {
                write(sockfd, buffer, strlen(buffer));
            }
        }   


        //se o servidor desconectar
        if (FD_ISSET(sockfd, &readfds)) {
            bzero(buffer, BUFFER_SIZE);
            ssize_t len = read(sockfd, buffer, sizeof(buffer));
            if (len <= 0) {
                printf("Server disconnected...\n");
                break;
            }
            printf("%s", buffer);
        }
    }

    close(sockfd);
    return 0;
}