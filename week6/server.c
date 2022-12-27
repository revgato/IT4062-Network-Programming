#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "lib_user_manage.h"

#define BUFF_SIZE 1024

void message_analyze(char *buff, char *username, char *password)
{
    int i = 0, j = 0, k = 0;
    char *token;
    token = strtok(buff, "-");
    while (token != NULL)
    {
        if (i == 0)
        {
            strcpy(username, token);
        }
        else if (i == 1)
        {
            strcpy(password, token);
        }
        token = strtok(NULL, "-");
        i++;
    }
    // username[strlen(username) - 1] = '\0';
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <Server Port>\n", argv[0]);
        return 0;
    }

    struct sockaddr_in server, client;
    int server_sock, client_sock, pid;
    char buff[BUFF_SIZE];
    int bytes_sent, bytes_received, sin_size;
    int port = atoi(argv[1]);
    char username[BUFF_SIZE], password[BUFF_SIZE], message[BUFF_SIZE];
    // int logged_in = 0;
    list *head = NULL;
    int quit = 0;
    read_file(&head);

    // Step 1: Construct a TCP socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        // Call socket() to create a socket
        perror("Error: ");
        return 0;
    }

    // Step 2: Bind address to socket
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("Port: %d\n", port);
    bzero(&server.sin_zero, 8);

    if ((bind(server_sock, (struct sockaddr *)&server, sizeof(server))) == -1)
    {
        // Call bind() to bind address to socket
        perror("Error: ");
        return 0;
    }

    // Step 3: Listen request from client
    if (listen(server_sock, 2) == -1)
    {
        // Call listen() to listen request from client
        perror("Error: ");
        return 0;
    }

    // Step 4: Communicate with client
    sin_size = sizeof(struct sockaddr_in);

    while (quit == 0)
    {
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client, &sin_size)) == -1)
        {
            perror("Error: ");
            return 0;
        }

        if ((pid = fork()) == -1)
        {
            close(client_sock);
            continue;
        }
        else if (pid > 0)
        {
            close(client_sock);
            continue;
        }
        else if (pid == 0)
        {
            printf("Connected to client %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
            while (1)
            {
                bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
                if (bytes_received < 0)
                {
                    perror("Error: ");
                    return 0;
                }
                buff[bytes_received] = '\0';
                message_analyze(buff, username, password);

                if (strcmp(username, "exit") == 0)
                {
                    close(client_sock);
                    quit = 1;
                    break;
                }

                reload_status_infile(head);
                login(head, username, password, message);
                write_file(head);


                bytes_sent = send(client_sock, message, strlen(message), 0);
                if (bytes_sent < 0)
                {
                    perror("Error: ");
                    return 0;
                }
            }

            close(client_sock);

            // continue;
        }

    }
    close(server_sock);
    write_file(head);
    free_list(head);
    return 0;
}
