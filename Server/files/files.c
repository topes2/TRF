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

            printf("Replace Key: %s, content: %s\n", key.dptr, content.dptr);

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

        printf("Key: %s, content: %s\n", key.dptr, content.dptr);

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

int getFile(int socket, int n, GDBM_FILE db){

}

/*
void listFiles(int socket, char *buffer, GDBM_FILE db){
    datum key,cont;
    gdbm_count_t t = gdbm_count(db,t);
    key = gdbm_firstkey(db);
    cont = gdbm_fetch(db,cont);

    int place = 0, si = (int)(t*streln(cont.dptr));
    char* buffer = malloc(si);
    
    strncpy(buffer + place,cont.dptr,strlen(cont.dptr));
    place += strlen(cont.dptr);

    while(key.dptr){
        gdbm_nextkey(db,key);
        cont = gdbm_fetch(db,cont);
            
        if((strlen(cont.dptr) > si)){ //making the buffer bigger if the question is bigger than the min value
            si = t*strlen(cont.dptr);
            char* buffer2 = malloc(t*si);
            strncpy(buffer2,buffer,strlen(buffer));
            buffer = buffer2;            
        }

        strncpy(buffer + place,cont.dptr,strlen(cont.dptr));
        place += strlen(cont.dptr);
        gdbm_nextkey(db,key);
    }
    
    sends(socket,buffer);
    writear(Socket,buffer);   
}
*/