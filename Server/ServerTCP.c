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

#include "backup/backup.h"
#include "network.h"

#define PORT 5555
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 1000

int main() {
    pthread_t time;
    //argumentos da thread
    volatile int flag = 0; //flag que determina o fim do programa
    backupArgs args;
    args.flag = &flag;

    //criacao da thread
    if (pthread_create(&time, NULL, Backup, &args)) {
        fprintf(stderr, "Erro ao criar thread principal\n");
        return 1;
    }

    int serverSocket, clientSockets[MAX_CLIENTS], maxSocket, activity, newSocket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    char message[1050]; //Para o chat
    
    //Configuração do servidor
    //Colocar valores 0 no array
    NetworkS(clientSockets, MAX_CLIENTS);

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

    
    //Ciclo para as conexões
    while(1){
        FD_ZERO(&readfds);
        maxSocket = serverSocket;
        FD_SET(serverSocket, &readfds);
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(clientSockets[i] > 0){
                FD_SET(clientSockets[i], &readfds);
            }

            //definir a maior socket
            if(maxSocket < clientSockets[i]){
                maxSocket = clientSockets[i];
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
            write(newSocket, "Connection sucesseful!\n", strlen("Connection sucesseful!\n"));

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clientSockets[i] == 0) {
                    clientSockets[i] = newSocket;
                    break;
                }
            }

        }    

        //chat
        for(int i = 0; i < MAX_CLIENTS; i++) {
            memset(buffer, 0, BUFFER_SIZE);
            if(FD_ISSET(clientSockets[i], &readfds)) {
                ssize_t len = read(clientSockets[i], buffer, BUFFER_SIZE);

                if(len <= 0) {
                    printf("Client %d left.\n", i + 1);
                    close(clientSockets[i]);
                    clientSockets[i] = 0;
                } else {
                    printf("Client %d: %s", i + 1, buffer);

                    // Vamos juntar as strings para mostrar quem disse o que
                    sprintf(message, "Client %d: %s", i + 1, buffer); // Use sprintf para formatar a mensagem corretamente

                    // Enviar a mensagem para todos os outros clientes
                    for(int j = 0; j < MAX_CLIENTS; j++) {
                        if(j != i && clientSockets[j] > 0) {
                            write(clientSockets[j], message, strlen(message));
                        }
                    }
                }
            }
        }
    }

    //close all
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clientSockets[i] > 0){
            close(clientSockets[i]);
        }
    }
    
    close(serverSocket);
    return 0;
}