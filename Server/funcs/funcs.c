#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <gdbm.h>
#include <unistd.h>

#include "../../configs.h"
#include "../network/network.h"
#include "funcs.h"
#include "../databases/db.h"

void clientExit(clientData *clients, int num){
    close(clients[num].socket);
    
    //starting values of socket and username
    clients[num].socket = 0;
    memset(clients[num].userName, 0, strlen(clients[num].userName)); 

    //Exit message
    printf("Client %d left.\n", num + 1);
}

//login
void clientLogin(int clientSocket, char *buffer, GDBM_FILE db){
    char userName[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    int codeLen = strlen(LOGIN_CODE);

    //find the char ':'
    char *points = strchr(buffer, ':');

    if(points != NULL){
        //Obtain pass and username
        strcpy(password, points + 1);
        *points = '\0';
        strcpy(userName, buffer + codeLen);

        int loginResult = loginDB(userName, password, db); //0 - cant find, 1 - wrong pass, username - login done

        if (loginResult == -1){ //Wrong password
            write(clientSocket, "1", strlen("1"));

        } else if(loginResult == 0){ //cant find register
            write(clientSocket, "2", strlen("2"));

            memset(buffer, 0, strlen(buffer));
            read(clientSocket, buffer, BUFFER_SIZE);

            printf("Client said: %s\n", buffer);
            
        } else { //done
            write(clientSocket, "3", strlen("3"));

        }

    } else {
        write(clientSocket, "Someting went wrong, please try again\n", strlen("Someting went wrong, please try again\n"));
    }
}