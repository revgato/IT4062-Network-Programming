#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE 1024

void message_analyze(char *message, char *number, char *character)
{
    int i = 0, j = 0, k = 0;
    char *token;
    token = strtok(message, "-");
    while (token != NULL)
    {
        if (i == 1)
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

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <Server IP> <Server Port>\n", argv[0]);
        return 0;
    }

    int client_sock;
    struct sockaddr_in server_addr;
    char buff[BUFF_SIZE];
    int bytes_sent, bytes_received, sin_size;
    char task[10];
    char username[BUFF_SIZE], password[BUFF_SIZE], message[BUFF_SIZE];
    char number[BUFF_SIZE], character[BUFF_SIZE];
    char SERV_IP[16];
    int SERV_PORT;

    strcpy(SERV_IP, argv[1]);
    SERV_PORT = atoi(argv[2]);

    // Step 1: Construct a UDP socket
    if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        // Call socket() to create a socket
        perror("Error: ");
        return 0;
    }

    // Step 2: Define the address of the server
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERV_IP);
    printf("Server IP: %s - Port: %d\n", SERV_IP, SERV_PORT);

    // Step 3: Connect to server
    sin_size = sizeof(struct sockaddr_in);
    bytes_sent = sendto(client_sock, "Hello", strlen("Hello") - 1, 0, (struct sockaddr *)&server_addr, sin_size);
    if (bytes_sent < 0)
    {
        perror("Error: ");
        return 0;
    }

    // Try to receive message from client
    bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
    if (bytes_received < 0)
    {
        perror("Error: ");
        return 0;
    }
    printf("Connected to server\n");

    // Start "login" process
    while (1)
    {
        // Send username to server
        // fgets(username, BUFF_SIZE, stdin);
        // bytes_sent = sendto(client_sock, username, strlen(username) - 1, 0, (struct sockaddr *)&server_addr, sin_size);
        // if (bytes_sent < 0)
        // {
        //     perror("\nError: ");
        //     return 0;
        // }

        // // Receive message from server
        // bytes_received = recvfrom(client_sock, message, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
        // message[bytes_received] = '\0';
        // if (bytes_received < 0)
        // {
        //     perror("\nError: ");
        //     return 0;
        // }
        // printf("\n%s\n", message);

        // Get username and password from user
        fgets(username, BUFF_SIZE, stdin);
        printf("Insert password\n");
        fgets(password, BUFF_SIZE, stdin);

        // Merge username and password to send to server
        strcpy(buff, username);
        strcat(buff, "-");
        strcat(buff, password);

        // sin_size = sizeof(struct sockaddr_in);
        bytes_sent = sendto(client_sock, buff, strlen(buff) - 1, 0, (struct sockaddr *)&server_addr, sin_size);
        if (bytes_sent < 0)
        {
            perror("Error: ");
            return 0;
        }

        // Receive message from server
        bytes_received = recvfrom(client_sock, message, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
        message[bytes_received] = '\0';
        if (bytes_received < 0)
        {
            perror("Error: ");
            return 0;
        }
        
        printf("%s\n", message);
        if(strcmp(message, "OK") == 0)
        {
            break;
        }
    }

    // Login successfully
    // Start "change password" process
    while(1){
        // Get new password from user
        fgets(password, BUFF_SIZE, stdin);
        bytes_sent = sendto(client_sock, password, strlen(password) - 1, 0, (struct sockaddr *)&server_addr, sin_size);
        if (bytes_sent < 0)
        {
            perror("Error: ");
            return 0;
        }

        // Receive message from server
        bytes_received = recvfrom(client_sock, message, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
        message[bytes_received] = '\0';
        if (bytes_received < 0)
        {
            perror("Error: ");
            return 0;
        }
        // Search sub-string "Goodbye" in message, if found, close connection
        if(strstr(message, "Goodbye") != NULL){
            printf("%s\n", message);
            break;
        }

        // Search sub-string "Error" in message, if found, continue
        if(strstr(message, "Error") != NULL){
            printf("%s\n", message);
            continue;
        }

        // Search sub-string "OK" in message, if found, continue
        if(strstr(message, "OK") != NULL){
            message_analyze(message, number, character);

            // If Number include @ character, this string is equal NULL
            if (strcmp(number, "@") != 0){
                printf("%s\n", number);
            }
            
            // If Character include @ character, this string is equal NULL
            if (strcmp(character, "@") != 0){
                printf("%s\n", character);
            }
            continue;
        }
    }

    // Close socket
    close(client_sock);
    return 0;
}