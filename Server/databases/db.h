#ifndef DB_H
#define DB_H

GDBM_FILE start_bd(char string[]);
int regs(char string[],char string1[], GDBM_FILE db);
int loginDB(char string[],char string1[], GDBM_FILE db);
int kick(char string[], GDBM_FILE db);

#endif