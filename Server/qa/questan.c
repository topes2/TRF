#include <gdbm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

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
    char aid[strlen(token)];
    strcpy(aid, token);
    token = strtok(NULL,":");

    char *id = malloc(strlen(aid) + strlen(userid) + 2);

    sprintf(id, "%s - %s", aid, userid);

    if(regs(id, answer, db) != 0){
        replace_value(aid, answer, db);
    }

    char *res = malloc(strlen("REGISTERED ") + strlen(aid) + 1);
    sprintf(res, "REGISTERED %s\n", aid);
    write(socket, res, strlen(res));
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
            write(socket, res, strlen(res));
        }
        qdb = gdbm_nextkey(db,q);
        q = qdb;
    }
}

/*
void list_questions(int socket,GDBM_FILE qdb, GDBM_FILE adb){ //list all the question and anwsers to the user that requested them
    char buffer[BUFFER_SIZE],*token;
    int counter = 0;
    datum keyq = gdbm_firstkey(qdb); // question key in the data base, wich is its id
    datum content = gdbm_fetch(qdb,keyq), contenta, nextkey; 
    
    //memset(buffer, 0, BUFFER_SIZE);
    sprintf(buffer, "QUESTION %s: %s", keyq.dptr,content.dptr); // make the string thats supposed to be sent to the user
    write(socket,buffer,strlen(buffer)+1); //send the string to the user

    printf("While start\n");
    while(keyq.dptr){
        printf("While1\n");
        sprintf(buffer, "(%s) %s", keyq.dptr,content.dptr); // make the string thats supposed to be sent to the user        
        datum keya = gdbm_firstkey(adb); //anwser key which has the id of answer and the id of the user that gave it
        while(keya.dptr){
            printf("while2\n");
            token = strtok(keya.dptr,"-");
            printf("%s %s \n",keyq.dptr,token);
            if(!strcmp(keyq.dptr,token)){
                token = strtok(NULL, "-");
                contenta = gdbm_fetch(adb,keya);
                sprintf(buffer,"%s %s ",token,contenta.dptr);
                write(socket,buffer,strlen(buffer)+1);
                counter++;
                nextkey = gdbm_nextkey(adb,keya);
                keya = nextkey;
                printf("existe 1 \n");
            }

            if(counter == 0){
                write(socket,"NO ANWSERS YET.", strlen("NO ANWSERS YET."));
            }
            nextkey = gdbm_nextkey(qdb,keyq);
            keyq = nextkey;
        }
        counter = 0;
    }

    printf("While end\n");
    write(socket,ENDQUESTIONS,strlen(ENDQUESTIONS));
}
*/

void list_questions(int socket,GDBM_FILE qdb, GDBM_FILE adb){
    datum ka,kac;
    datum kq,kqc;
    datum tk;
    int counter = 0;
    kq = gdbm_firstkey(qdb);
    while(kq.dptr){
        printf("while 1 \n");
        kqc = gdbm_fetch(qdb, kq); 
        if(!kqc.dptr){
            printf("break\n");
            break;
        }
        
        char *question = malloc(strlen(kq.dptr) + strlen(kqc.dptr) + 5);
        sprintf(question, "(%s) %s", kq.dptr, kqc.dptr);
        printf("Question: %s\n", question);
        //write(socket, question, strlen(question));

        ka = gdbm_firstkey(adb);
        while(ka.dptr){
            char *token = strtok(ka.dptr,"-");
            printf("token = %s key = %s \n",token,kq.dptr);
            if(!strcmp(token, kq.dptr)){
                counter++;
                token = strtok(NULL,"-");
                kac = gdbm_fetch(adb, ka); 
                if(!kac.dptr){
                    printf("breakA\n");
                    break;
                }                
                //printf("Token: %s\ncontent: %s\n", token, kac.dptr);
                if(!token){
                    break;
                }
                char *ans = malloc(strlen(token) + kac.dsize + 5);
                sprintf(ans, "(%s) %s\n", token, kac.dptr);
                //printf("ans: %s\n", ans);
                //write(socket, ans, strlen(ans));                 
            }

            if(counter == 0){
                write(socket,"NOT ANSWERED\n",strlen("NOT ANSWERED\n"));
            }

            tk = gdbm_nextkey(adb,ka);
            ka = tk;    
                  
            
        }
        tk = gdbm_nextkey(qdb,kq);
        kq = tk;        
        token = strtok(kq.dptr,"-");
    }

    printf("%s\n", ENDQUESTIONS);
    //write(socket, ENDQUESTIONS, strlen(ENDQUESTIONS));
}