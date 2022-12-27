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

    char SERV_IP[16];
    int SERV_PORT;

    strcpy(SERV_IP, argv[1]);
    SERV_PORT = atoi(argv[2]);

    // Step 1: Construct a UDP socket
    if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        // Call socket() to create a socket
        perror("\nError: ");
        return 0;
    }

    // Step 2: Define the address of the server
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERV_IP);
    printf("Server IP: %s - Port: %d\n", SERV_IP, SERV_PORT);

    // Step 3: Connect to server to receive client task
    sin_size = sizeof(struct sockaddr_in);
    char *msg = "GEt lactroi.txt 1237";
    bytes_sent = sendto(client_sock, msg, strlen(msg), 0, (struct sockaddr *)&server_addr, sin_size);
    if (bytes_sent < 0)
    {
        perror("\nError: ");
        return 0;
    }
    // bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
    // buff[bytes_received] = '\0';

    // if (bytes_received < 0)
    // {
    //     perror("\nError: ");
    //     return 0;
    // }

    
    close(client_sock);
    return 0;
}