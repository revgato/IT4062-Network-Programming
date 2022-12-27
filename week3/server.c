#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFF_SIZE 1024

void string_process(char *buff, char *character, char *number)
{
    // If received string include non-digit or non-alphabet character,
    // return error message
    int i;
    // Ignore the last character "\n"
    buff[strlen(buff) - 1] = '\0';

    for (i = 0; i < strlen(buff); i++)
    {
        if (isdigit(buff[i]) == 0 && isalpha(buff[i]) == 0)
        {
            strcpy(buff, "Error\0");
            return;
        }
    }
    // Split received string into character and number sub-string
    int j = 0;
    int k = 0;
    for (i = 0; i < strlen(buff); i++)
    {
        if (isdigit(buff[i]) != 0)
        {
            number[j] = buff[i];
            j++;
        }
        else
        {
            character[k] = buff[i];
            k++;
        }
    }
    // Copy character and number sub-string to buff
    strcpy(buff, number);
    strcat(buff, "\n");
    strcat(buff, character);
    buff[strlen(buff)] = '\0';
    // printf("\nbuff: %s", buff);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("\nUsage: %s <Server Port>", argv[0]);
        return 0;
    }
    struct sockaddr_in server, client_send, client_receive;
    int server_sock;
    char buff[BUFF_SIZE];
    int bytes_sent, bytes_received, sin_size;
    int port = atoi(argv[1]);
    char character[BUFF_SIZE], number[BUFF_SIZE];

    // Step 1: Construct a UDP socket

    if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        // Call socket() to create a socket
        perror("\nError: ");
        return 0;
    }

    // Step 2: Bind address to socket

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    printf("\nPort: %d", port);
    bzero(&server.sin_zero, 8);

    if ((bind(server_sock, (struct sockaddr *)&server, sizeof(server))) == -1)
    {
        // Call bind() to bind address to socket
        perror("\nError: ");
        return 0;
    }

    // Step 3: Connect to client

    // Connect to first client - client_send
    sin_size = sizeof(struct sockaddr_in);
    bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client_send, &sin_size);
    if (bytes_received < 0)
    {
        perror("\nError: ");
        return 0;
    }

    // Send message to client, mark this client is send client
    bytes_sent = sendto(server_sock, "send", strlen("send"), 0, (struct sockaddr *)&client_send, sin_size);
    printf("\nSend message");
    if (bytes_sent < 0)
    {
        perror("\nError: ");
        return 0;
    }

    printf("\nConnected to send client %s:%d", inet_ntoa(client_send.sin_addr), ntohs(client_send.sin_port));

    // Connect to second client - client_receive
    sin_size = sizeof(struct sockaddr_in);
    bytes_received = recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *)&client_receive, &sin_size);
    if (bytes_received < 0)
    {
        perror("\nError: ");
        return 0;
    }

    // Send message to client, mark this client is receive client
    bytes_sent = sendto(server_sock, "receive", strlen("receive"), 0, (struct sockaddr *)&client_receive, sin_size);
    if (bytes_sent < 0)
    {
        perror("\nError: ");
        return 0;
    }
    printf("\nConnected to receive client %s:%d", inet_ntoa(client_receive.sin_addr), ntohs(client_receive.sin_port));

    // Step 4: Communicate with clients
    printf("\nWaiting for client...");
    while (1)
    {
        // Set error, character and number string to null
        memset(character, '\0', (strlen(character) + 1));
        memset(number, '\0', (strlen(number) + 1));

        sin_size = sizeof(struct sockaddr_in);
        bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client_send, &sin_size);

        // Call recvfrom() to receive message from client
        if (bytes_received < 0)
        {
            perror("\nError: ");
        }
        else
        {
            buff[bytes_received] = '\0';
        }
        printf("\nServer received: %s", buff);
        // If received NULL string, send "Exit" message to receive client
        // and close connection
        if (buff[0] == '\n')
        {
            bytes_sent = sendto(server_sock, "Exit", strlen("Exit"), 0, (struct sockaddr *)&client_receive, sin_size);
            printf("\nClient disconnected!\n");
            break;
        }

        // Process received string
        string_process(buff, character, number);

        // printf("\nBuff size: %d", strlen(buff));
        bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client_receive, sin_size);
        if (bytes_sent < 0)
        {
            perror("\nError: ");
        }
    }
    close(server_sock);
    return 0;
}