#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 1024

void menu(int *choose)
{
    printf("1. Gui xau bat ky\n");
    printf("2. Gui noi dung mot file\n");
    scanf("%d%*c", choose);
}

void message_analyze(char *input, char *message, char *number, char *character)
{
    int i = 0, j = 0, k = 0;
    char *token;
    token = strtok(input, "-");
    while (token != NULL)
    {
        if (i == 0)
        {
            strcpy(message, token);
        }
        else if (i == 1)
        {
            strcpy(number, token);
        }
        else if (i == 2)
        {
            strcpy(character, token);
        }
        token = strtok(NULL, "-");
        i++;
    }
}

// This function is used to send file content to client line by line
void send_file(FILE *fp, int sockfd)
{
    char data[BUFF_SIZE] = {0};
    int n;
    printf("Sending file content to client...\n");
    while (fgets(data, BUFF_SIZE, fp) != NULL)
    {
        if (send(sockfd, data, sizeof(data), 0) == -1)
        {
            perror("[-]Error in sending file.");
            exit(1);
        }
        // Wait for server signal
        recv(sockfd, &n, sizeof(n), 0);
        if (n == 0)
        {
            printf("[-]Error in receiving data from server.\n");
            exit(1);
        }
        bzero(data, BUFF_SIZE);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <Server IP> <Server Port>\n", argv[0]);
        return 1;
    }

    int client_sock;
    char buff[BUFF_SIZE];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received;
    int SERVER_PORT = atoi(argv[2]);
    char message[100], number[100], character[100];
    int choose;

    // Step 1: Construct socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Step 3: Request to connect server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("\nError!Can not connect to sever! Client exit imediately! ");
        return 0;
    }

    // Step 4: Communicate with server
    menu(&choose);
    if (choose == 1)
    {
        // Send task to server
        strcpy(buff, "1");
        bytes_sent = send(client_sock, buff, BUFF_SIZE, 0);
        if (bytes_sent <= 0)
        {
            printf("\nConnection closed!\n");
            close(client_sock);
            return 0;
        }

        while (1)
        {
            // Send message
            printf("Insert string to send: ");
            memset(buff, '\0', (strlen(buff) + 1));
            fgets(buff, BUFF_SIZE, stdin);
            buff[strlen(buff) - 1] = '\0';
            msg_len = strlen(buff);
            if (msg_len == 0)
                break;

            bytes_sent = send(client_sock, buff, msg_len, 0);
            if (bytes_sent <= 0)
            {
                printf("Connection closed!\n");
                break;
            }

            // If the message is "exit", then we stop the client
            if (strcmp(buff, "exit") == 0)
            {
                printf("Client exit!\n");
                break;
            }

            if (strcmp(buff, "shutdown") == 0)
            {
                printf("Server shutdown!\n");
                break;
            }

            // Receive echo reply
            bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
            if (bytes_received <= 0)
            {
                printf("Error!Cannot receive data from sever!\n");
                break;
            }

            buff[bytes_received] = '\0';
            message_analyze(buff, message, number, character);

            // If message is "Error", then inputed string is include non-alphabet and
            // non-number character
            if (strcmp(message, "Error") == 0)
            {
                printf("%s\n", message);
            }
            else
            {
                // If Number include @ character, this string is equal NULL
                if (strcmp(number, "@") != 0)
                {
                    printf("%s\n", number);
                }

                // If Character include @ character, this string is equal NULL
                if (strcmp(character, "@") != 0)
                {
                    printf("%s\n", character);
                }
            }
        }
    }
    else if (choose == 2)
    {
        strcpy(buff, "2");
        bytes_sent = send(client_sock, buff, BUFF_SIZE, 0);
        if (bytes_sent <= 0)
        {
            printf("\nConnection closed!\n");
            close(client_sock);
            return 0;
        }
        char *filename = "input.txt";
        FILE *fp = fopen(filename, "r");
        if (fp == NULL)
        {
            perror("[-]Error in reading file.");
            exit(1);
        }

        send_file(fp, client_sock);
        printf("[+]File data sent successfully.\n");
        fclose(fp);
    }

    close(client_sock);
    return 0;
}
