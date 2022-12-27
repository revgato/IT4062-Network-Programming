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
    username[strlen(username) - 1] = '\0';
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <Server Port>\n", argv[0]);
        return 0;
    }

    struct sockaddr_in server, client_send, client_receive;
    int server_sock;
    char buff[BUFF_SIZE];
    int bytes_sent, bytes_received, sin_size;
    int port = atoi(argv[1]);
    char username[BUFF_SIZE], password[BUFF_SIZE];
    char message[BUFF_SIZE], number[BUFF_SIZE], character[BUFF_SIZE];
    int logged_in = 0;
    list *head = NULL;

    // Step 1: Construct a UDP socket
    if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        // Call socket() to create a socket
        perror("Error: ");
        return 0;
    }

    // Step 2: Bind address to socket

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    printf("Port: %d\n", port);
    bzero(&server.sin_zero, 8);

    if ((bind(server_sock, (struct sockaddr *)&server, sizeof(server))) == -1)
    {
        // Call bind() to bind address to socket
        perror("Error: ");
        return 0;
    }

    // Step 3: Connect to client
    sin_size = sizeof(struct sockaddr_in);
    bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client_send, &sin_size);
    if (bytes_received < 0)
    {
        perror("Error: ");
        return 0;
    }

    // Try to send a message to client
    bytes_sent = sendto(server_sock, "send", strlen("send"), 0, (struct sockaddr *)&client_send, sin_size);
    if (bytes_sent < 0)
    {
        perror("Error: ");
        return 0;
    }

    printf("Connected to client %s:%d\n", inet_ntoa(client_send.sin_addr), ntohs(client_send.sin_port));

    // Step 4: Communicate with client

    read_file(&head);

    while (logged_in == 0)
    {
        // Receive username and password from client
        bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client_send, &sin_size);
        if (bytes_received < 0)
        {
            perror("Error: ");
            return 0;
        }
        buff[bytes_received] = '\0';
        message_analyze(buff, username, password);

        // Try to login with username and password
        login(head, username, password, message);
        if (strcmp(message, "OK") == 0)
        {
            logged_in = 1;
        }

        // Send message to client
        bytes_sent = sendto(server_sock, message, strlen(message), 0, (struct sockaddr *)&client_send, sin_size);
        if (bytes_sent < 0)
        {
            perror("Error: ");
            return 0;
        }
    }

    // Start "change password" process
    while (1)
    {
        // Receive new password from client
        bytes_received = recvfrom(server_sock, password, BUFF_SIZE - 1, 0, (struct sockaddr *)&client_send, &sin_size);
        if (bytes_received < 0)
        {
            perror("Error: ");
            return 0;
        }
        password[bytes_received] = '\0';

        // If received "bye" message, close connection
        if (strcmp(password, "bye") == 0)
        {
            logout(head, username, message);
            // Send message to client
            bytes_sent = sendto(server_sock, message, strlen(message), 0, (struct sockaddr *)&client_send, sin_size);
            if (bytes_sent < 0)
            {
                perror("Error: ");
                return 0;
            }
            break;
        }

        // Change password and return 2 sub-string message
        change_password(head, username, password, message);

        // Send message to client
        bytes_sent = sendto(server_sock, message, strlen(message), 0, (struct sockaddr *)&client_send, sin_size);
        if (bytes_sent < 0)
        {
            perror("Error: ");
            return 0;
        }
    }

    // Step 5: Close socket
    close(server_sock);
    write_file(head);
    free_list(head);
    // Send goodbye message to client
    return 0;
}
