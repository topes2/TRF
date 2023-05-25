#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    volatile int* flag;
} backupArgs;

void* Backup(void* arg) {
    //argumentos
    backupArgs* args = (backupArgs*)arg;
    volatile int* flag = args->flag; 

    while(!*flag){
        printf("3 seconds have passed\n");
        sleep(3);
    }

    return NULL;
}

int main() {
    pthread_t  tTime;
    int i = 0;
    //argumentos da coisa
    volatile int flag = 0;
    backupArgs args;
    args.flag = &flag;

    if (pthread_create(&tTime, NULL, Backup, &args)) {
        fprintf(stderr, "Erro ao criar thread principal\n");
        return 1;
    }
    
    while(!flag){
        printf("%d\n", i);
        i++;
        sleep(1);

        if(i == 5){
            flag = 1;
        }
    }

    pthread_join(tTime, NULL);

    return 0;
}