#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../configs.h"
#include "funcs.h"

//Verify if the string is with the format
char* formatingLogin(char *buffer){
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

char* formatingQ_A(char *buffer){// switch to switch case if we have time 

    if(!strncmp("ASK ", buffer, strlen("ASK "))){ //new questions
        char *res = malloc(strlen(ASK_CODE) + strlen(buffer + strlen("ASK ")));
        sprintf(res, "%s:%s", ASK_CODE, buffer + strlen("ASK "));

        return res;

    } else if(!strncmp("ANSWER ", buffer, strlen("ANSWER "))){
        int Qnum;
        char *ans;
        if(sscanf(buffer, "ANSWER %d %[^\n]", &Qnum, ans) != 2){
            printf("Invalid command\n");
            return NULL; //comando invalido
        }
        
        char *res = malloc(strlen(ANSWER_CODE) + strlen(ans) + 2 + sizeof(Qnum));
        sprintf(res, "%s:%d:%s", ANSWER_CODE, Qnum, ans);
        
        return res;

    } else if(!strncmp("LISTQUESTIONS", buffer, strlen("LISTQUESTIONS"))){
        return LISTQUESTIONS_CODE;

    }

    if(!strncmp("PUTFILE ", buffer, strlen("PUTFILE "))){
        char *fileName = malloc(strlen(buffer) - strlen("PUTFILE ") + 1);
        int nBytes;

        if(sscanf(buffer, "PUTFILE %s %d", fileName, &nBytes) != 2){
            return NULL;
        }

        char *res = malloc(strlen(PUTFILES_CODE) + strlen(fileName) + sizeof(nBytes) + 3);   

        sprintf(res, "%s:%s:%d", PUTFILES_CODE, fileName, nBytes);

        return res;    
    }

    return NULL;
}

int login(int sockfd, char *buffer, char *loginCommand){
    write(sockfd, loginCommand, strlen(loginCommand));

    char userName[MAX_USERNAME_LENGTH];
    int codeLen = strlen(LOGIN_CODE);
    char *points = strchr(loginCommand, ':');

    *points = '\0';
    strcpy(userName, loginCommand + codeLen);

    memset(buffer, 0, strlen(buffer));
    read(sockfd, buffer, BUFFER_SIZE);
    int res = atoi(buffer);
    
    if(res == 1){
        printf("Error %s\n", userName);
        return 0;
    } 

    printf("Welcome %s\n", userName);
    return 1;
}

void QandA(int sockfd, char *buffer, char *res,int size){
    write(sockfd, res, strlen(res));
    if(!strncmp(res, ASK_CODE, strlen(ASK_CODE)) || !strncmp(res, ANSWER_CODE, strlen(ANSWER_CODE))){
        memset(buffer, 0, BUFFER_SIZE);
        read(sockfd, buffer, BUFFER_SIZE);

        printf("%s", buffer);
    } else if(!strcmp(res, LISTQUESTIONS_CODE)){
        do{
            memset(buffer, 0, BUFFER_SIZE);
            read(sockfd, buffer, BUFFER_SIZE);
           
            printf("%s", buffer);
            
        } while(strstr(buffer, ENDQUESTIONS) == NULL);
    }
}

//other
int getInput(char *buffer){
    memset(buffer, 0, strlen(buffer));
    fgets(buffer, BUFFER_SIZE, stdin);
            
    //fgets also gets \n so we need to take it out
    char *fgetsEnd = strchr(buffer, '\n');
    if(fgetsEnd != NULL){
        *fgetsEnd = '\0';
    }
    return strlen(buffer);

}   

