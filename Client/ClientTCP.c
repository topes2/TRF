#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>

#define PORT 5555
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed...");
        exit(-1);
    }

    server = gethostbyname("localhost");

    if (server == NULL) {
        printf("Server connection error...");
        exit(-1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection error...");
        exit(-1);
    }

    fd_set readfds;
    int max_fd;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        max_fd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        //Chat
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            bzero(buffer, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);

            if (strncmp("quit", buffer, 4) == 0) {
                printf("Leaving...\n");
                break;
            }

            write(sockfd, buffer, strlen(buffer));
        }

        if (FD_ISSET(sockfd, &readfds)) {
            bzero(buffer, BUFFER_SIZE);
            ssize_t len = read(sockfd, buffer, sizeof(buffer));
            if (len <= 0) {
                printf("Server disconnected...\n");
                break;
            }
            printf("%s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}