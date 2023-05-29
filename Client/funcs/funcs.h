#ifndef FUNCS_H
#define FUNCS_H

//login
char* formatting(char string[]);
int login(int sockfd, char *buffer, char* loginCommand);

//other
char* getInput(char *buffer);
#endif 