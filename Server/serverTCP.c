#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <gdbm.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>


#include "network/network.h"
#include "../configs.h"
#include "databases/db.h"
#include "funcs/funcs.h"
#include "qa/questan.h"
#include "files/files.h"
//#include "files/files.h"


int main(){
    int serverSocket, maxSocket, activity;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    fd_set readfds;

    //Client management
    client clients[MAX_CLIENTS];
    int flag = 0; //end 

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
    GDBM_FILE dbAttendance;
    GDBM_FILE dbQ;
    GDBM_FILE dbA;
    GDBM_FILE dbFiles;

    //Directories
    long pathMax = pathconf(".", _PC_PATH_MAX);
    char *cwd, *newDir;

    cwd = malloc(pathMax);
    newDir = malloc(pathMax);

    if(getcwd(cwd, pathMax) == NULL){
        printf("Directories error!\n");
        return -1;
    }

    strcpy(newDir, cwd);
    strcat(newDir, "/DatabasesFiles");

    create_directory(newDir);

    // start the dbs
    char *dbPath = malloc(pathMax + strlen("Attendance") + 2);
    sprintf(dbPath, "%s/Login", newDir);
    dbLogin = start_bd(dbPath);

    memset(dbPath, 0, strlen(dbPath) + 1);
    sprintf(dbPath, "%s/Attendance", newDir);
    dbAttendance = start_bd(dbPath);

    memset(dbPath, 0, strlen(dbPath) + 1);
    sprintf(dbPath, "%s/Q", newDir);
    dbQ = start_bd(dbPath);

    memset(dbPath, 0, strlen(dbPath) + 1);
    sprintf(dbPath, "%s/A", newDir);
    dbA = start_bd(dbPath);

    memset(dbPath, 0, strlen(dbPath) + 1);
    sprintf(dbPath, "%s/Files", newDir);
    dbFiles = start_bd(dbPath);

    memset(newDir, 0, strlen(newDir));

    strcpy(newDir, cwd);
    strcat(newDir, "/FilesUploaded");

    create_directory(newDir);

    free(cwd);
    free(newDir);
    free(dbPath);

    //start clock
    time_t start = time(NULL);

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
                int bytes = recs(clients[i].socket);
                char* bufferr = malloc(bytes);
                memset(bufferr,0,bytes+1);
                if(bytes <= 0){ //Client left
                    printf("%s left.\n", clients[i].userName);
                    close(clients[i].socket);
                    clients[i].socket = 0;
                    memset(clients[i].userName, 0, strlen(clients[i].userName));
                } 
                if(bytes > MAX_MESSAGE_LENGTH){
                    printf("tes3.1\n");
                    readar(clients[i].socket, bufferr, bytes);
                    printf("size of buffer: %d",strlen(bufferr));
                    printf("Buffer = \n %s \n",bufferr);
                    printf("tes3.2\n");
                }else if(bytes <= MAX_MESSAGE_LENGTH){
                    memset(bufferr, 0, bytes);
                    read(clients[i].socket, bufferr, bytes);
                }                
                
                if(!strncmp(bufferr, LOGIN_CODE, strlen(LOGIN_CODE))){ //Login
                    int loginRes = login(clients[i].userName, bufferr, dbLogin);                   
                    if(loginRes == 0){ //loggin done
                        write(clients[i].socket, "0", strlen("0"));
                        //Attendance
                        attendance(start, dbAttendance, clients[i].userName);
                    } else { //error
                        write(clients[i].socket, "1", strlen("1"));
                    }
                    
                    
                } 
                else { //Isloggedin
                    //readar
                    if(!strncmp(ASK_CODE, bufferr, strlen(ASK_CODE))){
                        printf("ASK - user: %s\n", clients[i].userName);
                        char *token = strtok(bufferr,":");
                        token = strtok(NULL,":");    
                        if(!search_question(token, dbQ)){
                            add_question(token,dbQ);
                            return_question(clients[i].socket, dbQ, token);
                        } else {
                            return_question(clients[i].socket, dbQ, token);
                        }

                    } else if(!strncmp(ANSWER_CODE, bufferr, strlen(ANSWER_CODE))){
                        char *token = strstr(bufferr, ":") + 1;
                        add_answer(token, clients[i].userName, dbA, clients[i].socket);
                        printf("ANSWER - user: %s\n", clients[i].userName);

                    } else if(!strncmp(LISTQUESTIONS_CODE, bufferr, strlen(LISTQUESTIONS_CODE))){
                        list_questions(clients[i].socket, dbQ, dbA);
                        printf("LISTQUESTIONS - user: %s\n", clients[i].userName);
                        
                    } else if(!strncmp(PUTFILES_CODE, bufferr, strlen(PUTFILES_CODE))){
                        putfile(bufferr, dbFiles);
                        printf("user: %s uploaded a file\n", clients[i].userName);

                    }else {
                        printf("the ends \n");
                    }
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