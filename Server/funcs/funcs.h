#ifndef FUNCS_H
#define FUNCS_H

//threads para os clientes

//Exits
void clientExit(clientData *clients, int num);


//login
void clientLogin(int clientSocket, char *buffer, GDBM_FILE db);

#endif