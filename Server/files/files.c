#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gdbm.h>

#include "../network/network.h"

int putfile(int socket, char *buffer, GDBM_FILE db){
    printf("putfiles, buffer: %s\n", buffer);
    char *fileName = strchr(buffer, ':') + 1;
    char *pt = fileName;
    char *bytes = strchr(pt, ':');
    *bytes = '\0';
    bytes++;

    //see if the file already exists
    datum key, content;

    key = gdbm_firstkey(db);

    int nBooks = 0;
    
    //key = num file / content = filename bytes
    while(key.dptr){
        content = gdbm_fetch(db, key);

        char *copy = malloc(strlen(content.dptr));
        sscanf(content.dptr, "%s ", copy);


        if(content.dptr && !strcmp(copy, fileName)){
            //book already in data base
            sends(socket, "1");
            writear(socket, "1");

            return 1;
        }

        datum nextkey = gdbm_nextkey(db, key);
        free(key.dptr);
        key = nextkey;
        nBooks++;
    }
    
    //book doesnt exist in data base

    //send message to client
    sends(socket, "0");
    writear(socket, "0");

    //see if the user has input done
    int bytesRead = recs(socket);
    readar(socket, buffer, bytesRead);
    
    if(strcmp(buffer, "1")){
        return -1;
    }

    bytesRead = recs(socket);
    readar(socket, buffer, bytesRead);
    printf("buffer: %s\n", buffer);

    //create new file
    char *path = malloc(strlen("FilesUploaded/") + strlen(fileName) + 1);
    sprintf(path, "FilesUploaded/%s", fileName);

    FILE *f = fopen(path, "w");

    fclose(f);

    //add name to data base
    char *nBooksString = malloc(21);
    sprintf(nBooksString, "%d", nBooks);

    key.dptr = nBooksString;
    key.dsize = strlen(nBooksString);

    char *dbCont = malloc(strlen(fileName) + 12);
    sprintf(dbCont, "%s %s", fileName, bytes);
    

    content.dptr = dbCont;
    content.dsize = strlen(dbCont);

    if(gdbm_store(db, key, content, GDBM_INSERT) != 0){
        printf("File already existed in data base\n");
        return -1;
    }

    //write for client UPLOADED file !!!!!!!!!!!!!!!!!!!
    return 0;
}

int getFile(int socket, int n, GDBM_FILE db){

}

void listFiles(int socket, char *buffer, GDBM_FILE db){

}