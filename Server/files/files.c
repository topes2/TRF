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
    buffer = buffer + strlen(GETFILES_CODE) + 1; //get only the number of fill
    datum key,cont;
    int size_file;
    key.dptr = buffer; 
    key.dsize = strlen(buffer); 
    cont = gdbm_fetch(db,key); //get the name and size of file

    if(cont.dptr == NULL){ // if the file doesnt exist then say so to client shouldnt happen
        //file doesnt exist
        sends(socket, "1");
        writear(socket, "1");
    }

    char* nas = malloc(cont.dsize + 1);
    strncpy(nas, cont.dptr, cont.dsize);
    nas[cont.dsize] = '\0';




    //sending the name and size to cliente
    sends(socket,nas);
    writear(socket,nas);

    char* token = strtok(nas," ");

    char* name = malloc(strlen(token) + strlen("FilesUploaded/") + 1);
    sprintf(name, "FilesUploaded/%s", token);

    token = strtok(NULL," ");
    char* size = malloc(strlen(token));
    size = token;
    int size_give = atoi(size);
    FILE *f = fopen(name,"r");

    if(f == NULL){
        printf("Error opening the file\n");
        sends(socket,"1");
        writear(socket,"1");
        return 0;
    }
    fseek(f, 0, SEEK_END); // seek to end of file
    size_file = ftell(f); // get current file pointer
    fseek(f, 0, SEEK_SET); // seek back to beginning of file 
    write(socket,size,strlen(size)); //used normal size since a file with a size with a number with more than 1024 digits is unlikely
    char* bufferfile = malloc(size_file);
    fread(bufferfile,1,size_file,f);   
    writear(socket,bufferfile);
    fclose(f);
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