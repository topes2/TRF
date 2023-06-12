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



void list_questions(int socket,GDBM_FILE qdb, GDBM_FILE adb){
    datum ka,kac;
    datum kq,kqc;
    datum tk;
    int counter = 0,cq,ca;
    gdbm_count(qdb,&cq);// the amount of entries in the questions data base
    gdbm_count(adb,&ca);// the amount of entries in the answers data base
    int ct = cq+ca; //count total
    kq = gdbm_firstkey(qdb);
    int min_size = (strlen(kq.dptr) + strlen("   NOT ANSWERED"));
    char* buffer_l = malloc((ct)*min_size);

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
            strncpy(buffer2,buffer1,strlen(buffer1));
            free(buffer1);
            buffer1 = buffer2;            
        } 
        strncpy(buffer2,question,strlen(question));

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
                    strncpy(buffer2,buffer1,strlen(buffer1));
                    free(buffer1);
                    buffer1 = buffer2;            
                }
                strncpy(buffer2,ans,strlen(ans));

            }
            tk = gdbm_nextkey(adb, ka);
            ka = tk;

            free(key_copy);
        }

        if(counter == 0){
            strncpy(buffer2,"   NOT ANSWERED\n",strlen("   NOT ANSWERED\n"));
        }

        counter = 0;
        tk = gdbm_nextkey(qdb,kq);
        kq = tk;        
    }
    sends(socket,buffer1);
    writear(socket, buffer1);
}
