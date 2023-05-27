#include "questan.h"

void add_question(char question[],GDBM_FILE db){ // adding the question to the data base
    int id = atoi(find_position(question,db));
    if(id != -1){
        regs(id,question,db);
    }else
        printf("Pergunta já foi inserida na base de dados."); //debug server side
}

void add_answer(char id[], char answer[],char userid[], GDBM_FILE db){ // adding the user provided answer to the data base
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

void list_questions(int socket,GDBM_FILE qdb, GDBM_FILE adb){ //list all the question and anwsers to the user that requested them
    int counter = 0;
    char buffer[BUFFER_SIZE],*token;
    datum keyq = gdbm_firstkey(qdb); // question key in the data base, wich is its id
    datum content = gdbm_fetch(qdb,keyq), contenta, nextkey; 

    memset(buffer, 0, BUFFER_SIZE);
    sprintf(buffer, "QUESTION %s: %s", keyq.dptr,content.dptr); // make the string thats supposed to be sent to the user
    write(socket,buffer,strlen(buffer)+1); //send the string to the user
    while(keyq.dptr){
        datum keya = gdbm_firstkey(adb); //anwser key which has the id of answer and the id of the user that gave it
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
}