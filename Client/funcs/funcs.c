#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../configs.h"
#include "funcs.h"

//Verify if the string is with the format
char* formating(char *buffer){
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    if(sscanf(buffer, "IAM %s WITHPASS %s", username, password) == 2){
        char* result = malloc(strlen(username) + strlen(password) + 2 + strlen(LOGIN_CODE)); //+ 2 because ':' and '\0'
        if(result == NULL){
            return NULL;
        }
        //code the server will receive
        sprintf(result, "%s%s:%s", LOGIN_CODE, username, password);

        return result;
    }

    return NULL;
}

int login(int sockfd, char *buffer, char *loginCommand){
    write(sockfd, loginCommand, strlen(loginCommand));

    memset(buffer, 0, strlen(buffer));
    read(sockfd, buffer, BUFFER_SIZE);

    printf("server: %s", buffer);

    return 1;
}

//other
void getInput(char *buffer){
    memset(buffer, 0, strlen(buffer));
    fgets(buffer, BUFFER_SIZE, stdin);
            
    //fgets also gets \n so we need to take it out
    char *fgetsEnd = strchr(buffer, '\n');
    if(fgetsEnd != NULL){
        *fgetsEnd = '\0';
    }
}   