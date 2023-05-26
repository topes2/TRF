#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <gdbm.h>

#include "db.h"

GDBM_FILE start_bd(char string[]){// recebe o nome da base de dados a abrir
    char* newString = malloc(strlen(string) + 5);
    strcat(newString, string);
    strcat(newString,".gdbm"); //adicionar o tipo de ficheiro ao nome

    GDBM_FILE db;

    //Abrir a base de dados
    db = gdbm_open(newString, 0, GDBM_WRCREAT, 0, NULL); //GDBM_WRCREAT serve para leitura e escrita e cria a base de dados se ela não existir
    if (db == NULL) {
        fprintf(stderr, "Couldn't open database: %s\n", gdbm_strerror(gdbm_errno));
        exit(1);
    }

    //Alterar permissões do arquivo
    if(chmod(newString, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0){
        printf("Could not change file permissions\n");
        exit(-1);
    }
        
    return db; //returnar a base de dados criada
}


void regs(char string[], char string1[], GDBM_FILE db){ //string == username string1 == password bd == base de dados
    datum key, content;
    //Organizar a chave e o conteudo
    key.dptr = string; //ID (index da DB)
    key.dsize = strlen(key.dptr) + 1; // + 1 por causa do \0

    content.dptr = string1; //Password
    content.dsize = strlen(content.dptr) + 1;

    int result = gdbm_store(db, key, content, GDBM_INSERT);
    if (result != 0) {
        fprintf(stderr, "Couldn't insert item into database: %s\n", gdbm_strerror(gdbm_errno));
    }
}


void kick(char string[],GDBM_FILE db){ //string == username string1 == password bd == base de dados
    datum key, content;
    //Organizar a chave e o conteudo
    key.dptr = string; //ID (index da DB)
    key.dsize = strlen(key.dptr) + 1; // + 1 por causa do \0

    int result = gdbm_delete(db, key);
    if (result != 0) {
        fprintf(stderr, "Couldn't delete item from database: %s\n", gdbm_strerror(gdbm_errno));
    }
}


char* login(char string[], char string1[], GDBM_FILE db){
    datum key, passkey, content;

    //Acessar ao conteudo da base de dados
    //Key
    key.dptr = string;
    key.dsize = strlen(key.dptr) + 1;

    passkey.dptr = string1;
    passkey.dsize = strlen(passkey.dptr);

    if(gdbm_exists(db, key)){
        content = gdbm_fetch(db, key);

        printf("Pass certa: %s\nPass obtida: %s\n", content.dptr, string1);

        if (!strcmp(content.dptr, passkey.dptr)){
            return key.dptr;
        } else {
            printf("Wrong PassWord \n");
            return NULL;
        }
    } else {
        fprintf(stderr, "Cant find username: %s\n", gdbm_strerror(gdbm_errno));
    }

    return NULL;
}