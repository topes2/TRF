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
    char counters[5]; //to save the counter in string form
    int counter = 1;//counter to add with the question
    q = gdbm_firstkey(db); //getting the first key from the data base
    while(q.dptr != NULL){//run trought the data base to know how many questions exist
        counter++;
        q1 = gdbm_nextkey(db,q);
        q = q1;
    }
    sprintf(counters, "%d", counter); //format the  string to add to the data base
    regs(counters,question,db); //add string to data base
}

int search_question(char question[], GDBM_FILE db){ //search the data base to check if the question exists
    datum q,q1; 
    datum qdb;
    char *token;
    q = gdbm_firstkey(db); //getting the first key in the data base
    while(q.dptr){
        qdb = gdbm_fetch(db,q); //getting the content of given key
        if(!strcmp(qdb.dptr,question)){ //comparing whats in the data base and what the we are searching
            return 1;
        }
        q1 = gdbm_nextkey(db,q); //getting the next key
        q = q1;
    }
    return 0;
}

void add_answer(char *answer, char *userid, GDBM_FILE db, int socket){ // adding the user provided answer to the given data base
    char *token = strtok(answer,":"); //split the answer being the format NUMBER:ANSWER
    
    //n Question
    char question_number[strlen(token)]; //get the question number from the token
    strcpy(question_number, token);
    printf("token = %s \n",token);
    
    token = strtok(NULL,":");//move the token foward

    char *id = malloc(strlen(token) + strlen(userid) + 2);
    sprintf(id, "%s:%s", answer, userid);

    if(regs(id, token, db) != 0){
        replace_value(id, token, db);
    }

    char *res = malloc(strlen("REGISTERED ") + strlen(question_number) + 1);
    sprintf(res, "REGISTERED %s\n", question_number);

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
    if(gdbm_exists(db,key)){
    gdbm_delete(db,key);
    }else
    return;
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

    buffer_1[place - 1] = '\0';
    sends(socket,buffer_1);
    writear(socket, buffer_1);
}