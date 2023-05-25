#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "backup.h"

//Funcao que verifica o tempo e cria um backup apos um certo tempo
void* Backup(void* arg){
    //argumentos
    backupArgs* arguments = (backupArgs*) arg;
    volatile int* flag = arguments->flag;
    
    //logs
    FILE *f = fopen("backup.log", "w"); 

    while(!(*flag)){
        sleep(300);
        printf("Starting backup\n");

        //Criacao do backup

    }

    fclose(f);
    return NULL;
}