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
    int loggedin = 0, firstmsg = 1; //0 nao fez log in, 1 fez

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
                } else {
                    write(sockfd, userPass, strlen(userPass));
                    memset(buffer, 0, strlen(buffer));
                    read(sockfd, buffer, BUFFER_SIZE); //leitura do resultado do login(server)
                    
                    if(!strcmp("1", buffer)){ //Password incorreta
                        printf("Wrong password, please try again\n");
                    } else if (!strcmp("2", buffer)){ // registo
                        printf("Acount not found, would you like to register? (y/n)\n");
                        memset(buffer, 0, strlen(buffer));
                        fgets(buffer, BUFFER_SIZE, stdin);

                        if(buffer[0] == 'y'){
                            write(sockfd, "21", strlen("21"));
                            
                            memset(buffer, 0, strlen(buffer));
                            read(sockfd, buffer, BUFFER_SIZE);

                            printf("%s", buffer);

                        } else if(buffer[0] == 'n'){
                            printf("Disconecting...\n");
                            break;
                        } else {
                            printf("Incorret option, please type (y/n)\n");
                        }

                    } else { //loggin feito com sucesso
                        printf("Loggin in\n");

                        memset(buffer, 0, strlen(buffer));
                        read(sockfd, buffer, BUFFER_SIZE);

                        while (!strncmp("Welcome ", buffer, 8)){
                            printf("Please wait");
                            
                        }

                        printf("%s", buffer);
                        loggedin = 1;
                    }
                }
            } else if (loggedin){
                printf("A escrever no server:\n%s\n", buffer);
            }
        }   

        //se o servidor desconectar
        if (FD_ISSET(sockfd, &readfds)) {
            bzero(buffer, BUFFER_SIZE);
            ssize_t len = read(sockfd, buffer, BUFFER_SIZE);
            if (len <= 0) {
                printf("Server disconnected...\n");
                break;
            }

            if(loggedin || firstmsg){
                printf("%s", buffer);
                firstmsg = 0;
            }
        }
    }

    close(sockfd);
    return 0;
}