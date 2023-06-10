#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <gdbm.h>

#include "db.h"

GDBM_FILE start_bd(char string[]){
    char* newString = malloc(strlen(string) + 6);
    strcat(newString, string);
    strcat(newString,".gdbm"); //Add type to the name

    GDBM_FILE db;

    //Abrir a base de dados
    db = gdbm_open(newString, 0, GDBM_WRCREAT, 0, NULL); //GDBM_WRCREAT - Open or create if it doesnt exist the db 
    if (db == NULL) {
        fprintf(stderr, "Couldn't open database: %s\n", gdbm_strerror(gdbm_errno));
        exit(1);
    }

    //Change permisions so it can open
    if(chmod(newString, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0){
        printf("Could not change file permissions\n");
        exit(-1);
    }
        
    return db; 
}

int regs(char string[], char string1[], GDBM_FILE db){ //string == username string1 == password bd == base de dados
    datum key, content;
    //Key and content for the db
    key.dptr = string; //ID (index da DB)
    key.dsize = strlen(key.dptr) + 1; // + 1 for the '\0'

    content.dptr = string1; //Password
    content.dsize = strlen(content.dptr) + 1;

    int result = gdbm_store(db, key, content, GDBM_INSERT);
    return result;
}

void replace_value(char string[], char string1[], GDBM_FILE db){
    datum key, content;
    //Key and content for the db
    key.dptr = string; //ID (index da DB)
    key.dsize = strlen(key.dptr) + 1; // + 1 for the '\0'

    content.dptr = string1; //Password
    content.dsize = strlen(content.dptr) + 1;
    gdbm_store(db, key, content, GDBM_REPLACE);
}

int kick(char string[], GDBM_FILE db){ //string == username string1 == password bd == base de dados
    datum key;

    key.dptr = string; //ID (index da DB)
    key.dsize = strlen(key.dptr) + 1; // + 1 por causa do \0

    
    if (gdbm_delete(db, key) != 0) {
        return -1;
    }

    return 0;
}

int loginDB(char string[], char string1[], GDBM_FILE db){
    datum key, passkey, content;

    //Acess data in db
    key.dptr = string;
    key.dsize = strlen(key.dptr) + 1;

    if(gdbm_exists(db, key)){
        content = gdbm_fetch(db, key);

        if (!strcmp(content.dptr, string1)){
            return 0; //correct
        } else {
            return -1; //Wrong password
        }
    } 
    
    return 1; //cant find
}