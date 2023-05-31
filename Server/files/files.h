#include <gdbm.h>

void add_file(GDBM_FILE db,char title[],char size[],char text[]);
void get_file(int socket,GDBM_FILE db,char id[]);
void list_files(int socket,GDBM_FILE db);