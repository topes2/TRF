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

    sends(socket, res);
    writear(socket, res); 
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
            sends(socket,res);
            writear(socket, res);
        }
        qdb = gdbm_nextkey(db,q);
        q = qdb;
    }
}


void list_questions(int socket,GDBM_FILE qdb, GDBM_FILE adb){
    datum ka,kac;
    datum kq,kqc;
    datum tk;
    int counter = 0,place = 0;
    gdbm_count_t ctq,cta;
    gdbm_count(qdb,&ctq);// the amount of entries in the questions data base
    gdbm_count(adb,&cta);// the amount of entries in the answers data base
    int cq = (int) ctq;
    int ca = (int) cta;
    int ct = cq+ca; //count total
    kq = gdbm_firstkey(qdb);
    int min_size = (5 * strlen("   NOT ANSWERED"));
    char* buffer_1 = malloc((ct)*min_size);
    while(kq.dptr){
        kqc = gdbm_fetch(qdb, kq);
        if(!kqc.dptr){
            break;
        } 
        char *question = malloc(strlen(kq.dptr) + strlen(kqc.dptr) + 5);
        sprintf(question, "(%s) %s \n", kq.dptr, kqc.dptr);
        if((strlen(question) > min_size)){ //making the buffer bigger if the question is bigger than the min value
            min_size = ct*strlen(question);
            char* buffer2 = malloc(ct*min_size);
            strncpy(buffer2,buffer_1,strlen(buffer_1));
            free(buffer_1);
            buffer_1 = buffer2;            
        }
        strncpy(buffer_1 + place,question,strlen(question));
        place += strlen(question);
        if(!kqc.dptr){
            break;
        }
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
                if((strlen(ans) > min_size)){ //making the buffer bigger if the answer is bigger than the min value
                    min_size = ct*strlen(ans);
                    char* buffer2 = malloc(ct*min_size);
                    strncpy(buffer2,buffer_1,strlen(buffer_1));
                    free(buffer_1);
                    buffer_1 = buffer2;            
                }
                strncpy(buffer_1+place,ans,strlen(ans));
                place += strlen(ans);
            }
            tk = gdbm_nextkey(adb, ka);
            ka = tk;
            free(key_copy);
        }
        if(counter == 0){
            strncpy(buffer_1+place,"   NOT ANSWERED\n",strlen("   NOT ANSWERED\n"));
            place += strlen("   NOT ANSWERED\n");
        }
        counter = 0;
        tk = gdbm_nextkey(qdb,kq);
        kq = tk;        
    }
    sends(socket,buffer_1);
    writear(socket, buffer_1);
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


