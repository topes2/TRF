#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <gdbm.h>

#include "network/network.h"
#include "../configs.h"
#include "databases/db.h"
#include "funcs/funcs.h"

int main(){
    int serverSocket, maxSocket, activity;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

    //Client management
    client clients[MAX_CLIENTS];
    int flag = 0; //end 

    char userName[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    //server configs
    NetworkS(clients, MAX_CLIENTS);
    serverSocket = Socket();
    server_addr = ServerConfig(server_addr, PORT);
    Bind(serverSocket, server_addr);
    Listen(serverSocket, MAX_CLIENTS);
    printf("Waiting for clients...\n");
    addr_len = sizeof(client_addr);

    //Data bases
    GDBM_FILE dbLogin;

    //start the dbs
    dbLogin = start_bd("Login");

    //Main cycle
    while(!flag){
        //Select
        if(prepareSelectServer(serverSocket, clients, &readfds, &maxSocket) < 0){
            break;
        }

        //New connection
        if(FD_ISSET(serverSocket, &readfds)){
            if(acceptNewConnection(serverSocket, clients, &client_addr, &addr_len) < 0){
                break;
            }
        }    

        //chat
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(FD_ISSET(clients[i].socket, &readfds)){
                memset(buffer, 0, strlen(buffer));
                ssize_t len = read(clients[i].socket, buffer, BUFFER_SIZE);

                if(len <= 0){ //Client left
                    printf("Client %d left.\n", i + 1);
                    close(clients[i].socket);
                    clients[i].socket = 0;
                } else if(!strncmp(buffer, LOGIN_CODE, strlen(LOGIN_CODE))){ //Login
                    int loginRes = login(clients[i].userName, buffer, dbLogin);

                    if(loginRes < 0){
                        write(clients[i].socket, "Someting went wrong, please try again\n", strlen("Someting went wrong, please try again\n"));
                    } else if(loginRes == 1){
                        write(clients[i].socket, "1", strlen("1"));
                    } else {
                        //String adjust
                        char *welcomeMSG = malloc(strlen("Welcome " + strlen(clients[i].userName))); 
                        sprintf(welcomeMSG, "Welcome %s", clients[i].userName); 
                        write(clients[i].socket, welcomeMSG, strlen(welcomeMSG));
                    }
                    
                } else { //Q&A

                } 
            }
        }
    }

    //closes
    //sockets
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].socket > 0){
            close(clients[i].socket);
        }
    }

    //others
    close(serverSocket);

    return 0;
}