#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define BACKLOG 2
#define BUFF_SIZE 1024

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
            printf("Error: Invalid input\n");
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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <Server Port>\n", argv[0]);
        return 1;
    }

    int listen_sock, conn_sock; /* file descriptors */
    char recv_data[BUFF_SIZE], send_data[BUFF_SIZE];
    int bytes_sent, bytes_received;
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    int sin_size;
    int PORT = atoi(argv[1]);
    int open_server = 1;

    // Step 1: Construct a TCP socket to listen connection request
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { /* calls socket() */
        perror("\nError: ");
        return 0;
    }

    // Step 2: Bind address to socket
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);              /* Remember htons() from "Conversions" section? =) */
    server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */
    if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    { /* calls bind() */
        perror("\nError: ");
        return 0;
    }

    // Step 3: Listen request from client
    if (listen(listen_sock, BACKLOG) == -1)
    { /* calls listen() */
        perror("\nError: ");
        return 0;
    }

    // Step 4: Communicate with client

    while (open_server)
    {
        // Accept request from client
        sin_size = sizeof(struct sockaddr_in);
        if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
        {
            perror("\nError: ");
            return 0;
        }
        printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

        // Receive task from client
        bytes_received = recv(conn_sock, recv_data, BUFF_SIZE, 0);
        recv_data[bytes_received] = '\0';

        // If task is split string
        if (strcmp(recv_data, "1") == 0)
        {
            // printf("Task: Split string\n");
            while (1)
            {
                bytes_received = recv(conn_sock, recv_data, BUFF_SIZE - 1, 0); /* receive data from client */
                if (bytes_received <= 0)
                {
                    printf("Connection closed\n");
                    close(conn_sock);
                    break;
                }
                recv_data[bytes_received] = '\0';
                printf("Received %d bytes: %s\n", bytes_received, recv_data);

                if (strcmp(recv_data, "exit") == 0)
                {
                    printf("Server closed\n");
                    open_server = 0;
                    close(conn_sock);
                    break;
                }

                split_string(recv_data, send_data);

                // Send response to client
                bytes_sent = send(conn_sock, send_data, strlen(send_data), 0); /* send to the client welcome message */
                printf("Sent bytes %d\n", bytes_sent);
                if (bytes_sent <= 0)
                {
                    printf("Connection closed\n");
                    close(conn_sock);
                    break;
                }
            }
        }else if(strcmp(recv_data, "2") == 0){
            // If task is receive file
            // printf("Task: Receive file\n");
            receive_file(stdout, conn_sock);
        }
        // Receive request from client
    }
}