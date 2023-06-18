#ifndef FUNCS_H
#define FUNCS_H

int login (char *username, char *buffer, GDBM_FILE db);
void attendance(time_t start, GDBM_FILE db, char *username);
int createAttendance(GDBM_FILE db);
//directory for data bases
int directory_exists(char* path);
void create_directory(char* path);

#endif