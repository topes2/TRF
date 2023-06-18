#include "../../configs.h"
#include <gdbm.h>

void add_question(char question[],GDBM_FILE db);
int search_question(char question[],GDBM_FILE db);
void add_answer(char *answer, char *userid, GDBM_FILE db, int socket);
int remove_answer(char *buffer, GDBM_FILE db);
void list_questions(int socket,GDBM_FILE qdb, GDBM_FILE adb);
void return_question(int socket, GDBM_FILE db, char question[]);