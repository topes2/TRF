#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "funcs.h"

char* formatting(char* string){
    char username[10];
    char password[51];

    if(sscanf(string, "IAM %9s WITHPASS %50s", username, password) == 2){
        char* result = malloc(strlen(username) + strlen(password) + 2); // + 2 pelos : e \0 
        if (result == NULL) {
            return NULL;
        }
        sprintf(result, "%s:%s", username, password);
        return result;
    }

    return NULL;
}