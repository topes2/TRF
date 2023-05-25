#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <gdbm.h>

int main(){
    GDBM_FILE db;
    datum key, content;
    gdbm_count_t items;

    //Abrir a base de dados
    db = gdbm_open("DB.gdbm", 0, GDBM_WRCREAT, 0, NULL); //GDBM_WRCREAT serve para leitura e escrita e cria a base de dados se ela não existir
    if (db == NULL) {
        fprintf(stderr, "Couldn't open database: %s\n", gdbm_strerror(gdbm_errno));
        return 1;
    }

    //Alterar permissões do arquivo
    if(chmod("DB.gdbm", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0){
        printf("Could not change file permissions\n");
        return -1;
    }

    

    //Organizar a chave e o conteudo
    key.dptr = "l51337"; //ID (index da DB)
    key.dsize = strlen(key.dptr) + 1; // + 1 por causa do \0

    content.dptr = "idk"; //Password
    content.dsize = strlen(content.dptr) + 1;
        
    //Inserir um novo valor na base de dados
    if(gdbm_store(db, key, content, GDBM_INSERT) != 0){ // 0 inseriu com sucesso, 1 já existe na db, -1 ocorreu um erro
        fprintf(stderr, "Couldn't insert item into database: %s\n", gdbm_strerror(gdbm_errno));
    }
    

    //organização da chave
    key.dptr = "l513371";
    key.dsize = strlen(key.dptr) + 1;

    //Remover records
    if(gdbm_delete(db, key) != 0){ //0 teve sucesso, -1 não encontrou o item ou ocorreu um erro
        fprintf(stderr, "Couldn't remove item from database: %s\n", gdbm_strerror(gdbm_errno));
    }

    //Acessar ao conteudo da base de dados
    //Key
    key.dptr = "l51337";
    key.dsize = strlen(key.dptr) + 1;

    content = gdbm_fetch(db, key);
    if(content.dptr == NULL){
        if(gdbm_errno == GDBM_ITEM_NOT_FOUND){
            fprintf(stderr, "key not found\n");
        } else {
            fprintf(stderr, "error: %s\n", gdbm_db_strerror(db));
        }
    } else {
        printf("User: %s\nPass: %s\n\n\n", key.dptr, content.dptr);
    }

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