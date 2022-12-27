#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE 1024

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
    int logged_in = 0;

    strcpy(SERV_IP, argv[1]);
    SERV_PORT = atoi(argv[2]);

    // Step 1: Construct a TCP socket
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

    // Step 3: Request to connect server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("\nError!Can not connect to sever! Client exit imediately! ");
        return 0;
    }

    while (logged_in == 0)
    {

        // Get username and password from user
        printf("Insert username\n");
        fgets(username, BUFF_SIZE, stdin);
        username[strlen(username) - 1] = '\0';
        printf("Insert password\n");
        fgets(password, BUFF_SIZE, stdin);
        password[strlen(password) - 1] = '\0';

        // Merge username and password to send to server
        strcpy(buff, username);
        strcat(buff, "-");
        strcat(buff, password);

        bytes_sent = send(client_sock, buff, BUFF_SIZE, 0);
        if (bytes_sent < 0)
        {
            perror("Error: ");
            close(client_sock);
            return 0;
        }

        if(strcmp(username, "exit") == 0){
            close(client_sock);
            return 0;
        }
        // Receive message from server
        memset(message, '\0', BUFF_SIZE);
        bytes_received = recv(client_sock, message, BUFF_SIZE, 0);
        message[bytes_received] = '\0';
        if (bytes_received < 0)
        {
            perror("Error: ");
            return 0;
        }

        printf("%s\n\n", message);
        if (strcmp(message, "OK") == 0)
        {
            logged_in = 1;
        }
    }

    // Close socket
    close(client_sock);
    return 0;
}