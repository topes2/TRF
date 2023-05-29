#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gdbm.h>

#include "../../configs.h"
#include "funcs.h"  


//Login
char* formatting(char string[]){
    char username[10];
    char password[51];

    if(sscanf(string, "IAM %9s WITHPASS %50s", username, password) == 2){
        char* result = malloc(strlen(username) + strlen(password) + 2); // + 2 because ':' and '\0' 
        if (result == NULL) {
            return NULL;
        }   
        
        //Code that the server will receive
        sprintf(result, "%s%s:%s", LOGIN_CODE, username, password); //formatar

        return result;
    }

    return NULL;
}

int login(int sockfd, char *buffer, char* loginCommand){
    write(sockfd, loginCommand, strlen(loginCommand)); //send the login code

    memset(buffer, 0, strlen(buffer));
    read(sockfd, buffer, BUFFER_SIZE); //read server (0 - login done, 1 - wrong pass, 2 - no resgister)

    if(!strcmp("2", buffer)){ //no register found
        printf("No account with that name was found, would you like to register? (y/n)\n");
        getInput(buffer);

        if(!strcmp("y", buffer)){
            write(sockfd, "21", strlen("21"));


        } else if("n", buffer){
            printf("Disconecting...\n");
            return -21;
        } else {
            printf("Invalid option, please type (y/n)\n");
        }
    }
}
//other
char* getInput(char *buffer){
    memset(buffer, 0, BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE, stdin);
            
    //fgets also gets \n so we need to take it out
    char *fgetsEnd = strchr(buffer, '\n');
    if(fgetsEnd != NULL){
        *fgetsEnd = '\0';
    }

    return buffer;
}   