#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>

#define BACKLOG 2
#define BUFF_SIZE 1024
int open_server = 1;

void split_string(char *input, char *output)
{
    int i = 0, j = 0, k = 0;
    char number[100], character[100];
    memset(number, '\0', 100);
    memset(character, '\0', 100);
    number[0] = '@';
    character[0] = '@';
    // Split received string into character and number sub-string

    for (i = 0; i < strlen(input); i++)
    {
        if (isdigit(input[i]) != 0)
        {
            number[j] = input[i];
            j++;
        }
        else if (isalpha(input[i]) != 0)
        {
            character[k] = input[i];
            k++;
        }
        else
        {
            // If received string include non-digit or non-alphabet character,
            // return error output
            strcpy(output, "Error");
            return;
        }
    }

    strcpy(output, "OK");
    strcat(output, "-");
    strcat(output, number);
    strcat(output, "-");
    strcat(output, character);
    return;
}

void receive_file(FILE *fp, int sockfd)
{
    int n;
    char data[BUFF_SIZE] = {0};
    while (1)
    {
        // Receive data from client and show it on screen
        n = recv(sockfd, data, BUFF_SIZE, 0);
        if (n <= 0)
        {
            break;
            return;
        }
        // Send server signal
        n = send(sockfd, &n, sizeof(n), 0);
        if (n == 0)
        {
            printf("[-]Error in sending signal to client.\n");
            exit(1);
        }
        printf("%s", data);
        memset(data, '\0', BUFF_SIZE);
    }
    return;
}

void *client_handler(void *arg)
{
    int clientfd;
    int sendBytes, recvBytes;
    char recv_data[BUFF_SIZE], send_data[BUFF_SIZE];

    pthread_detach(pthread_self());
    clientfd = (intptr_t)arg;
    while (1)
    {
        recvBytes = recv(clientfd, recv_data, BUFF_SIZE, 0);
        if (recvBytes <= 0)
        {
            printf("Client disconnected\n");
            break;
        }
        recv_data[recvBytes] = '\0';
        // sendBytes = send(clientfd, buff, recvBytes, 0);
        // if(sendBytes <= 0){
        //     printf("Client disconnected\n");
        //     break;
        // }
        if (strcmp(recv_data, "1") == 0)
        {
            // printf("Task: Split string\n");
            while (1)
            {
                recvBytes = recv(clientfd, recv_data, BUFF_SIZE - 1, 0); /* receive data from client */
                if (recvBytes <= 0)
                {
                    printf("Connection closed\n");
                    close(clientfd);
                    break;
                }
                recv_data[recvBytes] = '\0';
                // printf("Received %d bytes: %s\n", recvBytes, recv_data);

                if (strcmp(recv_data, "exit") == 0)
                {
                    printf("Bye client!\n");
                    break;
                }

                if (strcmp(recv_data, "shutdown") == 0){
                    printf("Server is shutting down\n");
                    pthread_exit(NULL);
                    break;
                }

                split_string(recv_data, send_data);

                // Send response to client
                sendBytes = send(clientfd, send_data, strlen(send_data), 0); /* send to the client welcome message */
                // printf("Sent bytes %d\n", sendBytes);
                if (sendBytes <= 0)
                {
                    printf("Connection closed\n");
                    close(clientfd);
                    break;
                }
            }
        }
        else if (strcmp(recv_data, "2") == 0)
        {
            // If task is receive file
            // printf("Task: Receive file\n");
            receive_file(stdout, clientfd);
        }
    }
    close(clientfd);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <Server Port>\n", argv[0]);
        return 1;
    }

    int listenfd, connfd; /* file descriptors */
    char recv_data[BUFF_SIZE], send_data[BUFF_SIZE];
    int bytes_sent, bytes_received;
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    int sin_size;
    int PORT = atoi(argv[1]);
    int open_server = 1;
    pthread_t tid;

    // Step 1: Construct a TCP socket to listen connection request
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { /* calls socket() */
        perror("\nError: ");
        return 0;
    }

    // Step 2: Bind address to socket
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);              /* Remember htons() from "Conversions" section? =) */
    server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */
    if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    { /* calls bind() */
        perror("\nError: ");
        return 0;
    }

    // Step 3: Listen request from client
    if (listen(listenfd, BACKLOG) == -1)
    { /* calls listen() */
        perror("\nError: ");
        return 0;
    }

    // Step 4: Communicate with client

    sin_size = sizeof(struct sockaddr_in);

    while (open_server)
    {
        // Accept request from client
        if ((connfd = accept(listenfd, (struct sockaddr *)&client, &sin_size)) == -1)
        {
            perror("\nError: ");
            return 0;
        }
        printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

        pthread_create(&tid, NULL, &client_handler, (void *)(intptr_t)connfd);

    }
    close(listenfd);
    return 0;
}