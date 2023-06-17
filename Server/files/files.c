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
            char *newString = malloc(strlen(filename) + strlen(pt) + 2);
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

        char *newString = malloc(strlen(filename) + strlen(pt) + 2);
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

int getFile(int socket,char *buffer, GDBM_FILE db){// RECEIVES code : number of file in bd
    //get the number of the file wanted from the user
    char *num = malloc(21);
    num = buffer + strlen(GETFILES_CODE) + 1;
    //get the name of the file
    datum key, content;

    key.dptr = num;
    key.dsize = strlen(key.dptr);

    content = gdbm_fetch(db, key);

    //Obtain file size
    char *contentCopy = strdup(content.dptr);
    contentCopy[content.dsize] = '\0';

    //send to client
    sends(socket, contentCopy);
    writear(socket, contentCopy);

    //get file size and name
    strtok(contentCopy, " ");
    char *fileSize = strtok(NULL, " ");

    char *filename = malloc(content.dsize - strlen(fileSize)); //we dont do +1 because there is a space is there
    strncpy(filename, content.dptr, content.dsize - strlen(fileSize) - 1);
    filename[content.dsize - strlen(fileSize)] = '\0';

    //Read file contents and create a buffer

    char file[strlen("FilesUploaded/") + strlen(filename) + 1];
    sprintf(file, "FilesUploaded/%s", filename);
    
    FILE *f = fopen(file, "r");

    if(f == NULL){
        printf("file\n");
    }

    int bytes = atoi(fileSize);
    char fileBuffer[bytes + 1];

    fread(fileBuffer, 1, bytes, f);
    fileBuffer[bytes] = '\0';
    
        
    //send file content to client
    sends(socket,fileBuffer);
    writear(socket,fileBuffer);

    fclose(f);
}

void listFiles(int socket, GDBM_FILE db){
    datum key,cont;
    gdbm_count_t t;
    gdbm_count(db, &t);
    int ti = (int) t;
    int ss = strlen(cont.dptr); // ss = starter size

    key = gdbm_firstkey(db);
   
    if(gdbm_errno == GDBM_ITEM_NOT_FOUND){
        sends(socket, "1");
        writear(socket, "1");
        return;
    }

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