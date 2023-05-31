#include <string.h>
#include <gdbm.h>

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

    if(res < 1){ //wrong password
        return 1;
    } else if(res > 0){ //registo
        if(regs(userName, password, db) != 0){
            return -1; //Error inserting
        }
    } 

    strcpy(username, userName);
    return 0; //login done
}