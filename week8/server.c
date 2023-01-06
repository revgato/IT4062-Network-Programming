#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "hardware.h"
#include "communicate.h"

// Handle client task
void *client_handle(void *);
struct sockaddr_in *client; /* server's address information */

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <Server Port>\n", argv[0]);
        return 1;
    }
    int listenfd, *connfd;
    int SERV_PORT = atoi(argv[1]);
    struct sockaddr_in server; /* server's address information */
    int sin_size;
    pthread_t tid;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { /* calls sockets() */
        perror("\nError: ");
        return 0;
    }

    // Prevent "Address already in use" error
    int option = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERV_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

    if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("\nError: ");
        return 0;
    }

    if (listen(listenfd, BACKLOG) == -1)
    {
        perror("\nError: ");
        return 0;
    }

    sin_size = sizeof(struct sockaddr_in);
    client = malloc(sin_size);
    while (1)
    {
        connfd = malloc(sizeof(int));
        if ((*connfd = accept(listenfd, (struct sockaddr *)client, &sin_size)) == -1)
            perror("\nError: ");

        printf("You got a connection from %s\n", inet_ntoa((*client).sin_addr)); /* Print client's IP */

        // For each client, spawns a thread, and the thread handles the new client
        pthread_create(&tid, NULL, &client_handle, connfd);
    }

    close(listenfd);
    return 0;
}

void *client_handle(void *arg)
{
    int connfd;
    int bytes_received, bytes_send;
    char buff[BUFF_SIZE + 1];
    conn_msg conn_message;

    connfd = *((int *)arg);
    free(arg);
    pthread_detach(pthread_self());
    bytes_received = recv(connfd, &conn_message, sizeof(conn_message), 0); // blocking
    if (bytes_received < 0)
        perror("\nError: ");
    else if (bytes_received == 0)
        printf("Connection closed.\n");
    else
    {
        save_log(inet_ntoa((*client).sin_addr), conn_message.data.hwr);
        strcpy(buff, "Hardware information received. Thank you ");
        strcat(buff, conn_message.data.hwr.nodename);
        conn_message = make_message_text(buff);
        bytes_send = send(connfd, &conn_message, sizeof(conn_msg), 0);
        if (bytes_send < 0)
            perror("\nError: ");
    }
    close(connfd);
}
