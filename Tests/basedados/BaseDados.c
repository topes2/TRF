#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <gdbm.h>

#include "BaseDados.h"


GDBM_FILE start_bd(char string[]){// recebe o nome da base de dados a abrir

        char* newString = malloc(strlen(string) + 5);
        strcat(newString, string);
        strcat(newString,".gdbm");

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
        
        return db;
}


void regs(char string[],char string1[],GDBM_FILE db){ //string == username string1 == password bd == base de dados
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


char* login(char string[],char string1[],GDBM_FILE db){
    datum key,passkey, content;

    //Acessar ao conteudo da base de dados
    //Key
    key.dptr = string;
    key.dsize = strlen(key.dptr) + 1;

    passkey.dptr = string1;
    passkey.dsize = strlen(passkey.dptr);

    if(gdbm_exists(db, key)){
        content = gdbm_fetch(db, key);
        if(strcmp(content.dptr,passkey.dptr)){
            return key.dptr;
        }else
            fprintf(stderr, "Wrong PassWord: %s\n", gdbm_strerror(gdbm_errno));
    }else
        fprintf(stderr, "Cant find username: %s\n", gdbm_strerror(gdbm_errno));
}

int main(){

    gdbm_count_t items;

    printf("Items in DB: %llu\n", items);
    char name[10] = "yourmom";
    GDBM_FILE db = start_bd(name);
    char name1[8] = "topes1";
    char name2[4] = "gato";
    regs(name1,name2,db);

    printf("Items in DB: %llu\n", items);
    return 0;
}

/*
int main(){
    GDBM_FILE db;
    datum key, content;
    gdbm_count_t items;


    // Verificar se a Base de dados está vazia
    if(gdbm_count(db, &items) != 0){
        fprintf(stderr, "Couldn't get count: %s\n", gdbm_strerror(gdbm_errno));
        return -1;
    }

    printf("Items in DB: %llu\n", items);

    //Acesso sequencial á base de dados (tipo limpar)
    key = gdbm_firstkey (db);
   while (key.dptr)
    {
        datum nextkey;

        //Remover a chave
        //Remover records
        if(gdbm_delete(db, key) != 0){ //0 teve sucesso, -1 não encontrou o item ou ocorreu um erro
            fprintf(stderr, "Couldn't remove item from database: %s\n", gdbm_strerror(gdbm_errno));
        }        

        //Obter a próxima chave
        nextkey = gdbm_nextkey (db, key);
        //Limpar a memoria 
        free (key.dptr);
        //Mudar a chave
        key = nextkey;
    }

    // Verificar se a Base de dados está vazia
    if(gdbm_count(db, &items) != 0){
        fprintf(stderr, "Couldn't get count: %s\n", gdbm_strerror(gdbm_errno));
        return -1;
    }

    printf("Items in DB: %llu\n", items);

    //Close the data base
    gdbm_close(db);
    return 0;
}
*/