#include <gdbm.h>

void add_file(GDBM_FILE db,char title[],char size[]);
void get_file(GDBM_FILE db,char title[]);
void list_files(int socket,GDBM_FILE db);