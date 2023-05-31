#include <gdbm.h>
#include <string.h>
#include <unistd.h>
#include "../databases/db.h"
#include "../../configs.h"

void add_file(GDBM_FILE db,char title[],char size[],char text[]){
    datum fid,t,k,k1; //file name + id and text
    int counter = 0;
    char string[strlen(title) + 5 + strlen(size)], counters[5];
        while(k.dptr != NULL){
        counter++;
        k1 = gdbm_nextkey(db,k);
        k = k1;
    }
    sprintf(counters, "%d", counter);
    strcat(string,counters);
    strcat(string,"-");
    strcat(string,title);
    strcat(string,"-");
    strcat(string,size);
    regs(string,text,db);
}

void get_file(int socket,GDBM_FILE db,char id[]){
    datum f,f1,ft;//f = file, f1 = content, ft = temp file handler
    char *token; // holds the id to compare
    f = gdbm_firstkey(db);
    token = strtok(f.dptr,"-");
    while(f.dptr){
        if(!strcmp(token,id)){ //compare the token with the id on the db to the id given
            f1 = gdbm_fetch(db,f);
            char buffer[strlen(f.dptr) + strlen("\n") + strlen(f1.dptr)];
            strcat(buffer,f1.dptr);
            strcat(buffer,"\n");
            strcat(buffer,f1.dptr);
            write(socket,buffer,strlen(buffer));
        }
        ft = gdbm_nextkey(db,f);
        f = ft;
        token = strtok(f.dptr,"-");
    }
    write(socket,"FILE NOT FOUND", strlen("FILE NOT FOUND"));    
}

void list_files(int socket,GDBM_FILE db){
    datum f,f1,ft;
    char *token;
    f = gdbm_firstkey(db);
    f1 = gdbm_fetch(db,f);
    char buffer[strlen(f.dptr) + strlen(f1.dptr)];
    while(f.dptr){
        write(socket,f.dptr,strlen(f.dptr));
        write(socket,f1.dptr,strlen(f1.dptr));
        ft = gdbm_nextkey(db,f);
        f = ft;
        f1 = gdbm_fetch(db,f);
    }
    write(socket,STOP_LOOP,strlen(STOP_LOOP));
}