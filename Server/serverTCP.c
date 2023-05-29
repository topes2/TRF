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
#include "backup/backup.h"
#include "databases/db.h"
#include "funcs/funcs.h"

int main(){
    int serverSocket, maxSocket, activity;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

    //Client management
    clientData clients[MAX_CLIENTS];

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

    //threads for backup
    pthread_t time;
    volatile int flag = 0; //flag that ends the program
    backupArgs args;
    args.flag = &flag;

    //thread creation
    if (pthread_create(&time, NULL, Backup, &args)) {
        printf("Thread creation error\n");
        return 1;
    }

    //Main cycle
    while(!flag){
        //Select
        if(prepareSelectServer(serverSocket, clients, &readfds, &maxSocket) < 0){
            break;
        }

        //New connection
        handleNewConnection(serverSocket, &readfds, dbLogin, &client_addr, &addr_len, clients, &flag);  

        /* remover !!!!!!!!!!!!!!!!!!!!!!!!!!!
        //chat
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(FD_ISSET(clients[i].socket, &readfds)){
                //Leitura dos clientes
                memset(buffer, 0, strlen(buffer));
                ssize_t len = read(clients[i].socket, buffer, BUFFER_SIZE);


                if(len <= 0){ //Cliente exit
                    clientExit(clients, i);
                } else {
                    if(!strncmp(LOGIN_CODE, buffer, strlen(LOGIN_CODE))){ //User trying to login
                        clientLogin(clients[i].socket, buffer, dbLogin);
                    } else {
                            
                    }

                }   

            }
        }
        */
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