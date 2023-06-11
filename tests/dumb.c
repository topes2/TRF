#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Server/databases/db.h"
#include <gdbm.h>

void main(){
    GDBM_FILE qdb = start_bd("A");
    int cq;
    gdbm_count(qdb,cq);
    printf("%d \n",cq);
}


/*
void list_questions(int socket,GDBM_FILE qdb, GDBM_FILE adb){
    datum ka,kac;
    datum kq,kqc;
    datum tk;
    int counter = 0,cq,ca;
    gdbm_count(qdb,cq);
    gdbm_count(adb,ca);
    kq = gdbm_firstkey(qdb);
    char* buffer = malloc((cq+cq)*strlen(kq.dptr) + strlen("   NOT ANSWERED"));

    while(kq.dptr){
        kqc = gdbm_fetch(qdb, kq); 

        if(!kqc.dptr){
            break;
        }
        
        char *question = malloc(strlen(kq.dptr) + strlen(kqc.dptr) + 5);
        sprintf(question, "(%s) %s \n", kq.dptr, kqc.dptr);
        sends(socket,question);
        writear(socket, question);

        ka = gdbm_firstkey(adb);
        while(ka.dptr){
           
            char *key_copy = strdup(ka.dptr);
            char *token = strtok(key_copy,":");
           
            
            if(!strcmp(token, kq.dptr)){
                counter++;
                token = strtok(NULL,":");
                //user
                kac = gdbm_fetch(adb, ka);
               
                if(!kac.dptr){
                    break;
                }
                if(!token){
                    break;
                }
                
                char *ans = malloc(strlen(token) + kac.dsize + 8);
                sprintf(ans, "   (%s) %s\n", token, kac.dptr);
                sends(socket, ans);
                writear(socket, ans);                 
            }
            tk = gdbm_nextkey(adb, ka);
            ka = tk;

            free(key_copy);
        }

        if(counter == 0){
            sends(socket,"   NOT ANSWERED\n");
            writear(socket,"   NOT ANSWERED\n");
        }

        counter = 0;
        tk = gdbm_nextkey(qdb,kq);
        kq = tk;        
    }
    sends(socket,ENDQUESTIONS);
    writear(socket, ENDQUESTIONS);
}
*/