#ifndef FUNCS_H
#define FUNCS_H

char* formatingLogin(char *buffer);
char* formatingQ_A(char *buffer);

int login(int sockfd, char *buffer, char *loginCommand);
void getInput(char *buffer);

#endif 