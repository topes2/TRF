#ifndef defines_H
#define defines_H

//includes para o codigo
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <gdbm.h>

#include "database/db.h"
#include "backup/backup.h"
#include "network.h"

//servidor
#define PORT 5555
#define BUFFER_SIZE 2048
#define MAX_CLIENTS 100

//Q&A
#define MAX_ANSWER 100
#define MAX_LEGTH 1024

//users
#define MAX_USERNAME_LENGTH 10
#define MAX_PASSWORD_LENGTH 51
#define LOGIN_CODE "(!+{';!$^)"

#endif 