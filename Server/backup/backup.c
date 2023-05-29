#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "backup.h"

//Function for the backup
void* Backup(void* arg){
    //args
    backupArgs* arguments = (backupArgs*) arg;
    volatile int* flag = arguments->flag;
    
    //logs
    FILE *f = fopen("backup.log", "w"); 

    while(!(*flag)){
        sleep(300);
        printf("Starting backup\n");

        //Backup creation

    }

    fclose(f);
    return NULL;
}