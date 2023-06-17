#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gdbm.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../databases/db.h"
#include "funcs.h"
#include "../../configs.h"

int login(char *username, char *buffer, GDBM_FILE db){
    char userName[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

     int codeLen = strlen(LOGIN_CODE);

    //find the char ':'
    char *points = strchr(buffer, ':');

    if(points == NULL){
        return -1;
    } 

    //Obtain pass and username
    strcpy(password, points + 1);
    *points = '\0';
    strcpy(userName, buffer + codeLen);

    int res = loginDB(userName, password, db);//1 - cant find, -1 - wrong pass, 0 - login done
    
    if(res == -1){ //wrong password
        return 1;

    } else if(res == 1){ //registo
        if(regs(userName, password, db) != 0){
            return 1; //Error inserting
        }
    } 
    
    memset(username, 0, strlen(username) + 1);
    strcpy(username, userName);
    return 0; //login done
}

void attendance(time_t start, GDBM_FILE db, char *username){
    time_t end = time(NULL);
    int time = (int) difftime(end, start);

    struct tm *local_time = localtime(&end);

    int day = local_time->tm_mday;
    int month = local_time->tm_mon + 1;
    int year = local_time->tm_year + 1900;
    char *key = malloc(10 + strlen(username)); //10 because date and -
    sprintf(key, "%d/%d/%d-%s", day, month, year, username);

    if(time >= ABSENT_TIME ){ //-1
        regs(key, "-1", db);
    } else if (time >= LATE_TIME){ //0
        regs(key, "0", db);
    } else { //present 1
        regs(key, "1", db);
    }
}

int createAttendance(GDBM_FILE db){
    FILE *f = fopen("FilesUploaded/Attendance.txt", "w");

    if(f == NULL){
        return 1;
    }

    datum key, content;
    key = gdbm_firstkey(db);

    while(key.dptr){
        content = gdbm_fetch(db, key);

        char *cont = malloc(key.dsize + content.dsize + 1);

        strncpy(cont, key.dptr, key.dsize);
        strcat(cont, "-");
        strncat(cont, content.dptr, content.dsize);
        strcat(cont, "\n");
        
        fwrite(cont, sizeof(char), strlen(cont), f);
        
        key = gdbm_nextkey(db, key);
    }

    fwrite("END OF CONTENT", sizeof(char), strlen("END OF CONTENT"), f);

    fclose(f);
    return 0;
}

//Directories
int directory_exists(char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

void create_directory(char* path) {
    if(!directory_exists(path)) {
        mkdir(path, 0700);
    }
}