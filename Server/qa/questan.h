#include "../../configs.h"
#include <gdbm.h>

void add_question(char question[],GDBM_FILE db);
int search_question(char question[],GDBM_FILE db);
void add_answer(char id[], char answer[],char userid[], GDBM_FILE db);
void remove_answer(char id[],char userid[],GDBM_FILE db);
void list_questions(int socket,GDBM_FILE qdb, GDBM_FILE adb);