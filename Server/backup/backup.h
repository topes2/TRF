#ifndef BACKUP_H
#define BACKUP_H

typedef struct {
    volatile int* flag; //flag para o fim do programa
} backupArgs;

void* Backup(void* arg);

#endif 