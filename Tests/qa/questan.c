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
        regs(question,id,db);
    }else
        printf("Pergunta já foi inserida na base de dados.");
}

void add_answer(char id[], char answer[],char userid[], GDBM_FILE db){ // add a reposta a base de dados
    char aid[strlen(id) + strlen(userid) + 2];
    strcat(aid,id);
    strcat(aid,userid);
    regs(aid,answer,db);
}



void main(){
    char questionbd[10] = "questions";
    start_bd(questionbd);
    char awnseruser[15] = "awnseruser";
    start_bd(awnseruser);
    char q1[] = "Gostas de homens?";



}

