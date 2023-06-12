#ifndef FUNCS_H
#define FUNCS_H

char* formatingLogin(char *buffer);
char* formating(char *buffer);

int login(int sockfd, char *buffer, char *loginCommand);
void QandA(int sockfd, char *buffer, char *res);
void files(int sockfd, char *buffer, char *res);
int put_file(int socket,char* buffer);
int fileExists(char *filename, int *size);

int getInput(char *buffer);

void sends(int socket, char* buffer); // send size
int recs(int socket); //receive size

int readar(int socket, char* buffer,int size);
void writear(int socket,char* buffer);

#endif 