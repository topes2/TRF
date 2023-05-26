#include <stdio.h>
#include "../database/db.h"
#include <string.h>
#include <stdlib.h>
#include <gdbm.h>


#define MAX_ANSWER 100
#define MAX_LEGTH 1024


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
    char token;
    datum keyq = gdbm_firstkey(qdb); // chave da pergunta, que vai ter o id
    while(keyq.dptr){
        datum keya = gdbm_firstkey(adb); //cahve da resposta que vai ter o id+userid
        while(keya.dptr){
            char temp[strlen(keya.dptr)];
            token = strtok(keya.dptr,"-");
            if(strcmp(keyq.dptr,token)){
                write(socket,"QUESTION %c",keyq.dptr,"");         //TODO: acabar esta funcao, apenas tem de fazer write da perguntra e resposta, not that hard tbn
            }          


        }

    }





}//se na houver uma resposta para um pe rgunta temos de enviar que "NOT ANSWER"



void main(){
    char questionbd[10] = "questions";
    start_bd(questionbd);
    char awnseruser[15] = "awnseruser";
    start_bd(awnseruser);
    char q1[] = "Gostas de homens?";



}

