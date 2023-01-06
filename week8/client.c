#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "hardware.h"
#include "communicate.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <Server IP> <Server Port>\n", argv[0]);
        return 0;
    }
    char buff[BUFF_SIZE + 1];
    int bytes_sent, bytes_received;
    char SERV_IP[16];
    int SERV_PORT;
    int client_sock;
    struct sockaddr_in server_addr; /* server's address information */
    hardware sys_info = make_hardware();
    conn_msg conn_message = make_message_hwr(sys_info);

    strcpy(SERV_IP, argv[1]);
    SERV_PORT = atoi(argv[2]);

    // Step 1: Construct socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Step 2: Specify server address
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERV_IP);

    // Step 3: Request to connect server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("\nError! Cannot connect to server! Client exit immediately! ");
        return 0;
    }

    // Step 4: Communicate with server
    bytes_sent = send(client_sock, &conn_message, sizeof(conn_message), 0);
    if (bytes_sent < 0)
    {
        perror("\nError: ");
    }
    else
    {
        // Receive message from server
        bytes_received = recv(client_sock, &conn_message, sizeof(conn_message), 0);
        if (bytes_received < 0)
            perror("\nError: ");
        else if (bytes_received == 0)
            printf("Connection closed.\n");
        else
        {
            printf("Message from server: %s\n", conn_message.data.text);
        }
    }
    close(client_sock);
    return 0;
}
