#include <gdbm.h>

GDBM_FILE start_bd(char string[]);
int regs(char string[],char string1[]);
void login(char string[],char string1[],GDBM_FILE db);
void kick(char string[],GDBM_FILE db);