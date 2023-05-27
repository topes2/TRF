#include <stdio.h>
#include "../database/db.h"
#include <string.h>
#include <stdlib.h>
#include <gdbm.h>


#define MAX_ANSWER 100
#define MAX_LEGTH 1024
#define BUFFER_SIZE 2048


void add_question(char question[],GDBM_FILE db){ // add a pergunta para a base de dados das perguntas
    int id = atoi(find_position(question,db));
    if(id != -1){
        regs(id,question,db);
    }else
        printf("Pergunta já foi inserida na base de dados.");
}

void add_answer(char id[], char answer[],char userid[], GDBM_FILE db){ // add a reposta a base de dados
    char aid[strlen(id) + strlen(userid) + 2];
    strcat(aid,id);
    strcat(aid,"-");
    strcat(aid,userid);
    regs(aid,answer,db);
}

void remove_answer(char id[],char userid[],GDBM_FILE db){
    datum key;
    char aid[strlen(id) + strlen(userid) + 2];
    strcat(aid,id);
    strcat(aid,userid);
    key.dptr = aid;
    key.dsize = strlen(key.dptr) + 1;
    gdbm_delete(db,key);
}


//usar o write
/*
socket
*/
void list_questions(int socket,GDBM_FILE qdb, GDBM_FILE adb){//listar todas as perguntas e respostas para o user que pediu
    int counter = 0;
    char buffer[BUFFER_SIZE],*token;
    datum keyq = gdbm_firstkey(qdb); // chave da pergunta, que vai ter o id
    datum content = gdbm_fetch(qdb,keyq), contenta, nextkey; 

    memset(buffer, 0, BUFFER_SIZE);
    sprintf(buffer, "QUESTION %s: %s", keyq.dptr,content.dptr); // fazer o string a enviar com a key da pergunta seguida da pergunta da maneira que o professor quer
    write(socket,buffer,strlen(buffer)+1); //enviar a string para o cliente
    while(keyq.dptr){
        datum keya = gdbm_firstkey(adb); //chave da resposta que vai ter o id+userid
        counter = 0;
        while(keya.dptr){
            token = strtok(keya.dptr,"-");
            if(!strcmp(keyq.dptr,token)){
                token = strtok(NULL, "-");
                contenta = gdbm_fetch(adb,keya);
                sprintf(buffer,"%s %s",token,contenta.dptr);
                write(socket,buffer,strlen(buffer)+1);
                counter++;
                nextkey = gdbm_nextkey(adb,keya);
                keya = nextkey;
            }
            if(counter == 0)
                write(socket,"NO ANWSERS YET.", strlen("NO ANWSERS YET."));
            nextkey = gdbm_nextkey(qdb,keyq);
            keyq = nextkey;
        }
    }





}//se na houver uma resposta para um pe rgunta temos de enviar que "NOT ANSWER"



void main(){
    char questionbd[10] = "questions";
    start_bd(questionbd);
    char awnseruser[15] = "awnseruser";
    start_bd(awnseruser);
    char q1[] = "Gostas de homens?";
    char r1[] = "no";



}

