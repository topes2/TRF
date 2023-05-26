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

#include "database/db.h"
#include "backup/backup.h"
#include "network.h"
#include "defines.h"

int main() {
    int serverSocket, maxSocket, activity, newSocket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    char userName[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    client clients[MAX_CLIENTS];
    
    //Configuração do servidor
    //Colocar valores 0 no array
    NetworkS(clients, MAX_CLIENTS);

    //Criar a socket TCP
    serverSocket = Socket();

    //Configurar o servidor addr
    server_addr = ServerConfig(server_addr, PORT);

    //Bind socket and addr
    Bind(serverSocket, server_addr);

    //Listen 
    Listen(serverSocket, MAX_CLIENTS);

    printf("Waiting for clients...\n");
    addr_len = sizeof(client_addr);

    //threads
    pthread_t time;
    //argumentos da thread
    volatile int flag = 0; //flag que determina o fim do programa
    backupArgs args;
    args.flag = &flag;

    //Data Bases 
    GDBM_FILE dbLogin;

    //iniciar a base de dados
    dbLogin = start_bd("Login");


    //criacao da thread
    if (pthread_create(&time, NULL, Backup, &args)) {
        fprintf(stderr, "Erro ao criar thread principal\n");
        return 1;
    }
    
    //Ciclo para as conexões
    while(1){
        FD_ZERO(&readfds);
        maxSocket = serverSocket;
        FD_SET(serverSocket, &readfds);
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(clients[i].socket > 0){
                FD_SET(clients[i].socket, &readfds);
            }

            //definir a maior socket
            if(maxSocket < clients[i].socket){
                maxSocket = clients[i].socket;
            }
        }

        activity = select(maxSocket + 1, &readfds, NULL, NULL, NULL);
        if(activity < 0){
            perror("Error in Select\n");
            break;
        }

        if(FD_ISSET(serverSocket, &readfds)){
            newSocket = accept(serverSocket, (struct sockaddr *)&client_addr, &addr_len);
            if(newSocket < 0){
                perror("Error accepting connection\n");
                exit(EXIT_FAILURE);
            } else {
                memset(buffer, 0, BUFFER_SIZE);
            }

            printf("New client connected, IP: %s, Port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            write(newSocket, "Welcome\nInsira o comando IAM username WITHPASS yourpassword\n", strlen("Welcome\nInsira o comando IAM username WITHPASS yourpassword\n"));

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socket == 0) {
                    clients[i].socket = newSocket;
                    break;
                }
            }

        }    

        //chat
        for(int i = 0; i < MAX_CLIENTS; i++) {
            memset(buffer, 0, BUFFER_SIZE);
            if(FD_ISSET(clients[i].socket, &readfds)) {
                ssize_t len = read(clients[i].socket, buffer, BUFFER_SIZE);

                if(len <= 0) {
                    printf("Client %d left.\n", i + 1); //trocar para o user !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    close(clients[i].socket);
                    clients[i].socket = 0; //colocar a string vazia ja que o utilizador saiu
                } else {
                    //verificar se e mensagem de loggin
                    if(!strncmp(LOGIN_CODE, buffer, strlen(LOGIN_CODE))){ //0 se for igual
                        memset(userName, 0, strlen(userName));
                        memset(password, 0, strlen(password));

                        int codeLen = strlen(LOGIN_CODE);
                        //encontrar os :
                        char *points = strchr(buffer, ':');
                        if(points != NULL){
                            strcpy(password, points + 1);
                            *points = '\0';
                            strcpy(userName, buffer + codeLen);

                            char *loginResult = login(userName, password, dbLogin);

                            if(!strcmp("-1", loginResult)){ //password incorreta    
                                write(clients[i].socket, "Incorrect password, please try again\n", strlen("Incorrect password, please try again\n"));
                                write(clients[i].socket, "1", strlen("1")); // enviamos 0 para a variavel logged in no client 

                            } else if(!strcmp("0", loginResult)){ //nao foi encontrado registo
                                write(clients[i].socket, "Your acount was not found in our servers, would you like to register?(y/n)\n", strlen("Your acount was not found in our servers, would you like to register?(y/n)\n"));
                                write(clients[i].socket, "2", strlen("2"));

                                memset(buffer, 0, strlen(buffer));
                                read(clients[i].socket, buffer, BUFFER_SIZE);

                                if(!strcmp(buffer, "y")){
                                    if(!regs(userName, password, dbLogin)){
                                        memset(buffer, 0, BUFFER_SIZE);
                                        sprintf(buffer, "Register completed! Welcome %s!", userName);
                                        write(clients[i].socket, buffer, strlen(buffer));
                                        strcpy(clients[i].userName, userName);
                                    } else{
                                        write(clients[i].socket, "Error in register please try again.\n", strlen("Error in register please try again.\n"));
                                    }
                                } 

                            } else { //login bem sucedido
                                memset(buffer, 0, BUFFER_SIZE);
                                sprintf(buffer, "Welcome %s!", userName);
                                write(clients[i].socket, buffer, strlen(buffer));
                                strcpy(clients[i].userName, userName);
                            }
                            
                        } else {
                            write(clients[i].socket, "Login error, please try again.\n", strlen("Login error, please try again.\n"));
                        }
                    }

                    printf("Client %d: %s\n", i + 1, buffer);//print da mensagem do cliente

                    // Enviar a mensagem para todos os outros clientes
                    for(int j = 0; j < MAX_CLIENTS; j++) {
                        if(j != i && clients[j].socket > 0) {
                            write(clients[j].socket, buffer, strlen(buffer));
                        }
                    }
                }
            }
        }
    }

    //close all
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].socket > 0){
            close(clients[i].socket);
        }
    }
    
    //fechar tudo
    gdbm_close(dbLogin);
    close(serverSocket);
    return 0;
}