#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gdbm.h>

int putfile(char *buffer, GDBM_FILE db){
    char *fileName = strchr(buffer, ':') + 1;
    char *pt = fileName;
    char *bytes = strchr(pt, ':');
    *bytes = '\0';
    bytes++;
    
    char *path = malloc(strlen("FilesUploaded/") + strlen(fileName));
    sprintf(path, "FilesUploaded/%s", fileName);

    //see if the file already exists
    datum key, content;

    key = gdbm_firstkey(db);


    int nBooks = 0;
    while(key.dptr){
        content = gdbm_fetch(db, key);

        if(!strcmp(content.dptr, fileName)){
            //book already in data base
            return 1;
        }

        datum nextkey = gdbm_nextkey(db, key);
        free(key.dptr);
        key = nextkey;
        nBooks++;
    }

    //book doesnt exist in data base

    FILE *f = fopen(path, "w");
    
    //client sends stuff


    fclose(f);

    //add name to data base
    char *nBooksString;
    sprintf(nBooksString, "%d", nBooks);

    key.dptr = nBooksString;
    key.dsize = strlen(nBooksString);

    content.dptr = fileName;
    content.dsize = strlen(fileName);

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