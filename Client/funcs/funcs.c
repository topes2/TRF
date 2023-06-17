#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>

#include "../network/network.h"
#include "../funcs/funcs.h"
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

char* formating(char *buffer){ // switch to switch case if we have time 

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
        
        char *res = malloc(strlen(ANSWER_CODE) + strlen(ans) + 12);
        sprintf(res, "%s:%d:%s", ANSWER_CODE, Qnum, ans);
        
        return res;

    } else if(!strcmp("LISTQUESTIONS", buffer)){
        return LISTQUESTIONS_CODE;

    } else if(!strncmp("REMOVE ", buffer, strlen("REMOVE "))){
        char *res = malloc(strlen(buffer + strlen("REMOVE ")));

        char *command = strtok(buffer, " "); 
        char *nquestion = strtok(NULL, " "); 
        char *user = strtok(NULL, " "); 

        sprintf(res, "%s:%s:%s", REMOVE_ENTRY, nquestion, user);

        return res;
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

    } else if (!strcmp("LISTFILES", buffer)){
        return LISTFILES_CODE;

    } else if(!strncmp("GETFILE ", buffer, strlen("GETFILE "))){
        int nFile;

        char *pt = buffer + strlen("GETFILE ");

        while(*pt != '\0'){
            if(!isdigit(*pt)){
                return NULL;
            }
            pt++;
        }

        //get the number
        if(sscanf(buffer + strlen("GETFILE "), "%d", &nFile) != 1){
            return NULL;
        }

        char *res = malloc(strlen(GETFILES_CODE) + 12); //\0, ':' and 10 digits max
        sprintf(res, "%s:%d", GETFILES_CODE, nFile);

        return res;
    }

    if(!strncmp("SHUTDOWN", buffer, strlen("SHUTDOWN"))){ //command to shutdown server
        return CLOSE_CODE;
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

void QandA(int sockfd, char *buffer, char *res){
    int rec;
    sends(sockfd,res);
    writear(sockfd, res);

    if(!strncmp(res, ASK_CODE, strlen(ASK_CODE)) || !strncmp(res, ANSWER_CODE, strlen(ANSWER_CODE)) || strncmp(res, REMOVE_ENTRY, strlen(REMOVE_ENTRY))){
        memset(buffer, 0, BUFFER_SIZE);
        rec = recs(sockfd);
        readar(sockfd, buffer, rec);
        printf("%s", buffer);
        
    } else if(!strcmp(res, LISTQUESTIONS_CODE)){
        rec = recs(sockfd);
        readar(sockfd, buffer, rec);

        printf("%s\nENDQUESTIONS\n", buffer);
       
    } 
}

void files(int sockfd, char *buffer, char *res){
    int rec;

    if(!strncmp(res, PUTFILES_CODE, strlen(PUTFILES_CODE))){
        char *pt = strchr(res + strlen(PUTFILES_CODE) + 1, ':'); //Bytes
        int bytes = atoi(pt + 1);
        
        char *filename = malloc(strlen(res + strlen(PUTFILES_CODE)) - strlen(pt + 1) + 1);
        *pt = '\0';
        strcpy(filename, res + strlen(PUTFILES_CODE) + 1);
        *pt = ':';

        //see if the file exists
        if(access(filename, F_OK) != -1){
            printf("File doesnt exist\n");
            return;
        }
        
        char *fileOpen = malloc(strlen(filename) + strlen("Client/") + 1);
        sprintf(fileOpen, "Client/%s", filename);

        FILE* f = fopen(fileOpen, "r");

        if(f == NULL){
            printf("Couldn't open file\n");
            return;
        }

        fseek(f, 0, SEEK_END); // move para o final do arquivo
        int size = (int) ftell(f); // obtem a posição atual do ponteiro do arquivo // + 1 -> \0
        
        if(bytes != size){
            printf("bytes dont match\n");
            return;
        }

        char *fileBuffer = malloc(size + 1);
        rewind(f); 

        /*
        if(bytes > 5000){
            printf("File is to big!\n");
            return;
        }
        */

        size_t result = fread(buffer, 1, size, f); //limite de 5k bytes 

        if(result != size) {
            printf("Reading error\n");

            free(buffer);
            fclose(f);
            return;
        }
        buffer[size] = '\0';

        //Anouncement
        sends(sockfd, res);
        writear(sockfd, res);

        fclose(f);

        //send buffer to server
        sends(sockfd, buffer);
        writear(sockfd, buffer);

        //get response
        int resp = recs(sockfd);
        readar(sockfd, buffer, resp);
        printf("%s", buffer);

        return;

    } else if(!strncmp(res, LISTFILES_CODE, strlen(LISTFILES_CODE))){
        //Anouncement
        sends(sockfd, res);
        writear(sockfd, res);

        int bytes = recs(sockfd);
        readar(sockfd, buffer, bytes);
        if(strcmp(buffer, "1")){
            printf("%s", buffer);
        } 
        printf("ENDFILES\n");
        return;
        
    } else if(!strncmp(res, GETFILES_CODE, strlen(GETFILES_CODE))){
        //Sending code plus the num of the file
        sends(sockfd, res);
        writear(sockfd, res);
        //Before the getfiles function starts on the server

        int size_received = recs(sockfd); //size of the string received not the file
        char name_size[size_received];

        readar(sockfd, name_size, size_received );
        name_size[size_received] = '\0'; // WTF ESTA A DAR MERDA
        
       

        printf("size rec: %d, name_size: %s, len: %d\n",size_received,  name_size, strlen(name_size));
        char *token = strtok(name_size," ");

        char filename[strlen(token)];
        strcpy(filename, token);

        token = strtok(NULL," ");
        
        char filesize[strlen(token) ];
        strcpy(filesize,token);

        //create file and buffer
        char filebuffer[atoi(filesize)];
        char file[strlen(filename) + 13 + strlen("Client/")]; //og value is 13
        
        token = strtok(filename, ".");
        char name[strlen(token) ];
        strcpy(name, token);
        token = strtok(NULL, ".");
        char extension[strlen(token)];
        strcpy(extension, token);

        sprintf(file, "Client/%s.%s", name, extension);
        printf("file: %s, len: %d\n", file, strlen(file));

        int i = 1;
        while(access(file, F_OK) != -1){
            
            sprintf(file, "Client/%s-%d.%s", name, i, extension);
            i++;
        }

        FILE* f = fopen(file, "w");

        //gets file content
        if(recs(sockfd) != atoi(filesize)){
            printf("Error different sizes!\n");
            return;
        }
        
        readar(sockfd, filebuffer, atoi(filesize));
        
        printf("file buffer: %s, len: %ld\n", filebuffer, strlen(filebuffer));
        
        fwrite(filebuffer, 1, atoi(filesize), f);

        fclose(f);
        return;
    }
}

//other
int getInput(char *buffer){      //reads the user input and returns the size of the user input
    memset(buffer, 0, strlen(buffer));
    fgets(buffer, BUFFER_SIZE, stdin);
            
    //fgets also gets \n so we need to take it out
    char *fgetsEnd = strchr(buffer, '\n');
    if(fgetsEnd != NULL){
        *fgetsEnd = '\0';
    }

    return strlen(buffer);
}   

void sends(int socket,char* buffer){
    int size = strlen(buffer);
    char sizes[11];
    sprintf(sizes, "%d\n", size);  
    write(socket,sizes,strlen(sizes));  
}

int recs(int socket){ //recieves the size of a message incoming to see how many times to read
    char* buffer = malloc(22);
    char c;
    int size;
    int reader = 0;
    while(c != '\n'){
        reader += read(socket, &c, 1);
        *buffer = c;
        buffer+=1;        
    }
    buffer -= reader;
    size = atoi(buffer);
    return size;
}