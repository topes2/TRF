#include <gdbm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "../network/network.h"
#include "questan.h"
#include "../databases/db.h"



void add_question(char question[],GDBM_FILE db){ // adding the question to the data base
    datum q,q1;
    char counters[5];
    int counter = 1;
    q = gdbm_firstkey(db);
    while(q.dptr != NULL){
        counter++;
        q1 = gdbm_nextkey(db,q);
        q = q1;
    }
    sprintf(counters, "%d", counter);
    regs(counters,question,db);
}

int search_question(char question[], GDBM_FILE db){ //verifica se existe na base de dados
    datum q,q1;
    datum qdb;
    char *token;
    q = gdbm_firstkey(db);
    while(q.dptr){
        qdb = gdbm_fetch(db,q);
        if(!strcmp(qdb.dptr,question)){
            return 1;
        }
        q1 = gdbm_nextkey(db,q);
        q = q1;
    }
    return 0;
}

void add_answer(char *answer, char *userid, GDBM_FILE db, int socket){ // adding the user provided answer to the data base
    char *token = strtok(answer,":");
    
    //n Question
    char aid[strlen(token)];
    strcpy(aid, token);

    token = strtok(NULL,":");

    char *id = malloc(strlen(token) + strlen(userid) + 2);
    sprintf(id, "%s:%s", answer, userid);

    if(regs(id, token, db) != 0){
        replace_value(id, token, db);
    }

    char *res = malloc(strlen("REGISTERED ") + strlen(aid) + 1);
    sprintf(res, "REGISTERED %s\n", aid);
    write(socket, res, strlen(res)); //didnt use writear becuase googolplex is impossible
}

void remove_answer(char id[], char userid[], GDBM_FILE db){//remover a resposta da base de dados
    datum key;
    char aid[strlen(id) + strlen(userid) + 2];
    strcat(aid,id);
    strcat(aid,userid);
    key.dptr = aid;
    key.dsize = strlen(key.dptr) + 1;
    gdbm_delete(db,key);
}

void return_question(int socket, GDBM_FILE db, char question[]){
    datum q,qc;
    datum qdb;
    q = gdbm_firstkey(db);
    while(q.dptr){
        qc = gdbm_fetch(db,q);
        if(!strcmp(qc.dptr,question)){
            char *res = malloc(strlen("QUESTION ") + strlen(q.dptr) + strlen(": ") + strlen(qc.dptr) + strlen("\n") + 1);
            sprintf(res, "QUESTION %s: %s\n", q.dptr, qc.dptr);
            printf("size res %d",strlen(res));
            sends(socket,res);
            writear(socket, res);
            printf("sent\n");
        }
        qdb = gdbm_nextkey(db,q);
        q = qdb;
    }
}


void list_questions(int socket,GDBM_FILE qdb, GDBM_FILE adb){
    datum ka,kac;
    datum kq,kqc;
    datum tk;
    int counter = 0;

    kq = gdbm_firstkey(qdb);
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
                sends(socket,ans);
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


//debug
void print_gdbm_contents(GDBM_FILE db) {
    datum key = gdbm_firstkey(db);

    while (key.dptr) {
        datum value = gdbm_fetch(db, key);
        printf("Key: %s\n", (char *)key.dptr);
        printf("Value: %s\n", (char *)value.dptr);
        free(value.dptr);
        datum next_key = gdbm_nextkey(db, key);
        free(key.dptr);
        key = next_key;
    }
}