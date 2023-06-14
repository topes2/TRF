#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gdbm.h>

#include "../network/network.h"

int putfile(int socket, char *buffer, GDBM_FILE db) {
    char *pt = strchr(buffer + strlen(PUTFILES_CODE) + 1, ':'); //Bytes
    int bytes = atoi(pt + 1);
        
    char *filename = malloc(strlen(buffer + strlen(PUTFILES_CODE)) - strlen(pt + 1) + 1);
    *pt = '\0';
    strcpy(filename, buffer + strlen(PUTFILES_CODE) + 1);
    *pt = ':';

    datum key, content;
    int exists = 0;
    key = gdbm_firstkey(db);

    while(key.dptr){
        content = gdbm_fetch(db, key);

        if(!strncmp(content.dptr, filename, strlen(filename))){
            exists = 1;
            //file already in database
            char *newString = malloc(strlen(filename) + strlen(pt + 1) + 2);
            sprintf(newString, "%s %s", filename, pt + 1);
            
            content.dptr = newString;
            content.dsize = strlen(content.dptr);

            printf("File %s replaced\n", key.dptr);

            //replace
            if(gdbm_store(db, key, content, GDBM_REPLACE) != 0){
                printf("Couldnt replace\n");
            }
            break;
        }

        key = gdbm_nextkey(db, key);
    }

    if(!exists){
        gdbm_count_t count;
        gdbm_count(db, &count);

        int nFiles = (int) count;
        
        char sInt[11];

        sprintf(sInt, "%d", nFiles);

        key.dptr = sInt;
        key.dsize = strlen(key.dptr);

        char *newString = malloc(strlen(filename) + strlen(pt + 1) + 2);
        sprintf(newString, "%s %s", filename, pt + 1);

        content.dptr = newString;
        content.dsize = strlen(newString);

        printf("New file uploaded\n");

        gdbm_store(db, key, content, GDBM_INSERT);
    }

    //write file
    char *dir = malloc(strlen(filename) + strlen("FilesUploaded/") + 1);
    sprintf(dir, "FilesUploaded/%s", filename);

    char *fileBuffer = malloc(bytes + 1);

    //read client stuff
    int bytesRead = recs(socket);
    readar(socket, fileBuffer, bytesRead);
    //W clears the file and opens in write mode
    FILE *f = fopen(dir, "w");

    if(f == NULL){
        printf("Couldnt open file\n");
        //write to client
        return -1;
    }

    //write stuff
    fprintf(f, "%s", fileBuffer);

    fclose(f);

    //send message to client
    char *sendString = malloc(strlen(filename) + strlen("UPLOADED ") + 2);
    sprintf(sendString, "UPLOADED %s\n", filename);

    sends(socket, sendString);
    writear(socket, sendString);

    return 0;
}

int getFile(int socket, char *buffer, GDBM_FILE db){
    datum key, content;

    key.dptr = buffer + strlen(GETFILES_CODE) + 1;
    key.dsize = strlen(key.dptr);

    //get the content
    content = gdbm_fetch(db, key);

    char *file = malloc(content.dsize + 2 + strlen("FILE ") + key.dsize);
    //String creation
    sprintf(file, "FILE %.*s %.*s\n", key.dsize, key.dptr, content.dsize, content.dptr);

    //send to client
    sends(socket, file);
    writear(socket, file);

    char *filename = malloc(content.dsize + 1 + strlen("FilesUploaded/"));

    char *token = strtok(content.dptr, " ");
    
    filename = malloc(strlen(token) + strlen("FilesUploaded/") + 1); 
    
    sprintf(filename, "FilesUploaded/%s", token);

    //open file
    FILE *f = fopen(filename, "r");

    fseek(f, 0, SEEK_END); // move para o final do arquivo
    int size = (int) ftell(f); // obtem a posição atual do ponteiro do arquivo // + 1 -> \0

    char *fileBuffer = malloc(size + 1);
    rewind(f);

    //read file
    fread(fileBuffer, 1, size, f);

    fclose(f);
    //send file
    sends(socket, fileBuffer);
    writear(socket,fileBuffer);

    return 0;
}


void listFiles(int socket, GDBM_FILE db){
    datum key,cont;
    gdbm_count_t t;
    gdbm_count(db, &t);
    int ti = (int) t;
    int ss = strlen(cont.dptr); // ss = starter size

    key = gdbm_firstkey(db);
    cont = gdbm_fetch(db,key);

    int place = 0, si = (ti * ss);
    char* buffer = malloc(si);

    while(key.dptr){
        cont = gdbm_fetch(db,key);

        if((strlen(cont.dptr) > ss)){ //making the buffer bigger if the question is bigger than the min value
            si = ti*strlen(cont.dptr);
            char* buffer2 = malloc(si + 4);
            memset(buffer2, 0, sizeof(buffer2));
            strncpy(buffer2, buffer, strlen(buffer));
            buffer = buffer2;
        }

        char *num = malloc(11);//digits
        strncpy(num, key.dptr, key.dsize);
        char *newString = malloc(strlen(num) + 4);
        sprintf(newString, "(%s) ", num);

        strcat(buffer, newString);
        strcat(buffer, cont.dptr);
        strcat(buffer, "\n");

        place += strlen(cont.dptr) + 1;
        key = gdbm_nextkey(db,key);
    }
    
    sends(socket, buffer);
    writear(socket, buffer);   
}