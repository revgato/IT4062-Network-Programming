#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "user_manage.h"
#include "message.h"
#include "communicate.h"

void *receive_message();
int client_sock;
struct sockaddr_in server_addr; /* server's address information */

int main()
{
    char buff[BUFF_SIZE + 1];
    user client_info;
    conn_msg message;
    int conn_msg_len, bytes_sent, bytes_received;
    pthread_t receive_thread, send_thread;

    // Step 1: Construct socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    // Step 3: Request to connect server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("\nError! Cannot connect to server! Client exit immediately! ");
        return 0;
    }

    // Step 4: Communicate with server

    // Send message
    printf("Input username: ");
    scanf("%s%*c", client_info.username);
    printf("Input password: ");
    scanf("%s%*c", client_info.password);

    bytes_sent = send(client_sock, &client_info, sizeof(client_info), 0);
    if (bytes_sent < 0)
        perror("\nError: ");

    bytes_received = recv(client_sock, &message, sizeof(message), 0);
    if (bytes_received < 0)
        perror("\nError: ");
    else if (bytes_received == 0)
        printf("Connection closed.");

    if (message.type == LOGIN_FAIL)
    {
        printf("Login fail\n");
    }
    else if (message.type == LOGIN_SUCCESS)
    {
        printf("Login successfully\n");
    }
    printf("Received from server: %s\n", message.data.text);
    fflush(stdout);
    // while (1)
    // {
    //     bytes_received = recv(client_sock, &message, sizeof(message), 0); // blocking
    //     fflush(stdout);
    //     if (bytes_received < 0)
    //     {
    //         perror("\nError: ");
    //         break;
    //     }
    //     else if (bytes_received == 0)
    //     {
    //         printf("Connection closed.");
    //         break;
    //     }
    //     print_message(message.data.msg);
    // }
    pthread_create(&receive_thread, NULL, &receive_message, NULL);
    // Step 4: Close socket
    close(client_sock);
    return 0;
}

void *receive_message()
{
    int bytes_received;
    conn_msg conn_message;

    pthread_detach(pthread_self());

    while ((bytes_received = recv(client_sock, &conn_message, sizeof(conn_message), 0)) > 0)
    {

        fflush(stdout);
        printf("received %d bytes\n", bytes_received);
        fflush(stdout);
        if (bytes_received < 0)
        {
            perror("\nError: ");
            // break;
        }
        else if (bytes_received == 0)
        {
            printf("Connection closed.");
            // break;
        }
        switch (conn_message.type)
        {
        case CHAT_MESSAGE:
            print_message(conn_message.data.msg);
            break;
        case CONN_MESSAGE:
            printf("%s\n", conn_message.data.text);
            break;
        }
    }
    close(client_sock);
}