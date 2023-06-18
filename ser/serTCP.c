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

    int ret;
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
                char* buffer = malloc(bytes);
                memset(buffer, 0, bytes+1);

                if(bytes <= 0){ //Client left
                    printf("%s left.\n", clients[i].userName);
                    close(clients[i].socket);
                    clients[i].socket = 0;
                    memset(clients[i].userName, 0, MAX_USERNAME_LENGTH);
                    strcpy(clients[i].userName, "GUEST");
                    break;
                }

                if(bytes > MAX_MESSAGE_LENGTH){
                    readar(clients[i].socket, buffer, bytes);
                }else if(bytes <= MAX_MESSAGE_LENGTH){
                    memset(buffer, 0, bytes);
                    read(clients[i].socket, buffer, bytes);
                }                
                
                if(!strncmp(buffer, LOGIN_CODE, strlen(LOGIN_CODE))){ //Login
                    int loginRes = login(clients[i].userName, buffer, dbLogin);                   
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
                    if(!strncmp(ASK_CODE, buffer, strlen(ASK_CODE))){
                        char *token = strtok(buffer,":");
                        token = strtok(NULL,":");    
                        if(!search_question(token, dbQ)){
                            add_question(token,dbQ);
                            return_question(clients[i].socket, dbQ, token);
                        } else {
                            return_question(clients[i].socket, dbQ, token);
                        }
                    } else if(!strncmp(ANSWER_CODE, buffer, strlen(ANSWER_CODE))){
                        char *token = strstr(buffer, ":") + 1;
                        add_answer(token, clients[i].userName, dbA, clients[i].socket);
                        printf("ANSWER - user: %s\n", clients[i].userName);

                    } else if(!strncmp(LISTQUESTIONS_CODE, buffer, strlen(LISTQUESTIONS_CODE))){
                        list_questions(clients[i].socket, dbQ, dbA);
                        printf("LISTQUESTIONS - user: %s\n", clients[i].userName);
                        
                    }else if(!strncmp(REMOVE_ENTRY,buffer, strlen(REMOVE_ENTRY))){ 
                        ret = remove_answer(buffer, dbA);
                        if(!ret){
                            sends(clients[i].socket, "Removed successfully\n");
                            writear(clients[i].socket, "Removed successfully\n");

                            printf("REMOVE - user: %s\n", clients[i].userName);
                        } else {
                            sends(clients[i].socket, "Couldn't find entry\n");
                            writear(clients[i].socket, "Couldn't find entry\n");
                        }
                        
                    } else if(!strncmp(PUTFILES_CODE, buffer, strlen(PUTFILES_CODE))){
                        putfile(clients[i].socket, buffer, dbFiles);
                        printf("PUTFILE - user: %s\n", clients[i].userName);

                    }else if(!strcmp(LISTFILES_CODE, buffer)){
                        listFiles(clients[i].socket, dbFiles);
                        printf("LISTFILES - user: %s\n", clients[i].userName);
                        
                    }else if(!strncmp(GETFILES_CODE, buffer, strlen(GETFILES_CODE))){
                        getFile(clients[i].socket, buffer, dbFiles);
                        printf("GETFILES - user: %s\n", clients[i].userName);

                    }else if(!strcmp(CREATE_ATTENDANCE_FILE_CODE, buffer)){
                        ret = createAttendance(dbAttendance); 
                        if(!ret){
                            //success
                            sends(clients[i].socket, "0");
                            writear(clients[i].socket, "0");
                            printf("CREATE ATTENDANCE - user: %s\n", clients[i].userName);
                        } else {
                            //failure
                            sends(clients[i].socket, "1");
                            writear(clients[i].socket, "1");
                        }


                    } else if (!strcmp(CLOSE_CODE, buffer)){ //closing 
                        printf("Closing server...\n");

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