#ifndef FUNCS_H
#define FUNCS_H

int login (char *username, char *buffer, GDBM_FILE db);
void attendance(time_t start, GDBM_FILE db, char *username);

#endif