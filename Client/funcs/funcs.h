#ifndef FUNCS_H
#define FUNCS_H

char* formatingLogin(char *buffer);
char* formating(char *buffer);

int login(int sockfd, char *buffer, char *loginCommand);
void QandA(int sockfd, char *buffer, char *res);

int getInput(char *buffer);

void sends(int socket, char* buffer); // send size
int recs(int socket); //receive size

#endif 