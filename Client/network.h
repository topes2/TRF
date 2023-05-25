//Funções relacionadas com a redes (client side)
int Socket();
struct hostent* ServerbyName(char name[]);
struct sockaddr_in ServerSetup(struct sockaddr_in serv_addr, int port, struct hostent *server);
void Connect(int sockfd, struct sockaddr_in serv_addr);