#ifndef BACKUP_H
#define BACKUP_H

typedef struct {
    volatile int* flag;
} backupArgs;

void* Backup(void* arg);

#endif 