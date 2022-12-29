#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "user_manage.h"
#include "message.h"
#include "communicate.h"

// Handle client task
void *client_handle(void *);

int main(){
    int listenfd, *connfd;
    struct sockaddr_in server;  /* server's address information */
    struct sockaddr_in *client; /* server's address information */
    int sin_size;
    pthread_t tid;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {   /* calls sockets() */
        perror("\nError: ");
        return 0;
    }
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
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
    while(1)
    {
        connfd = malloc(sizeof(int));
        if ((*connfd = accept(listenfd, (struct sockaddr *)client, &sin_size)) == -1)
            perror("\nError: ");

        printf("You got a connection from %s\n", inet_ntoa((*client).sin_addr));  /* Print client's IP */

        // For each client, spawns a thread, and the thread handles the new client
        pthread_create(&tid, NULL, &client_handle, connfd);
    }

    close(listenfd);
    return 0;
}

void *client_handle(void *arg)
{
    int connfd;
    int bytes_sent, bytes_received;
    char buff[BUFF_SIZE + 1];

    connfd = *((int *)arg);
    free(arg);
    pthread_detach(pthread_self());
}