void print_gdbm_contents(GDBM_FILE db);

int putfile(int socket, char *buffer, GDBM_FILE db);
int getFile(int socket, int n, GDBM_FILE db);
void listFiles(int socket, GDBM_FILE db);

