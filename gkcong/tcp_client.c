#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 1024

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
    bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
    if (bytes_received <= 0)
    {
        printf("\nError!Cannot receive data from sever!\n");
        close(client_sock);
        return 0;
    }

    while (1)
    {
        // Find EOF substring in buff
        char *eof = strstr(buff, "EOF");
        if (eof != NULL)
        {
            // Replace EOF substring with '\0'
            *eof = '\0';
            printf("%s\n", buff);
            break;
        }
        printf("%s", buff);
        memset(buff, '\0', (strlen(buff) + 1));
        bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
        if (bytes_received <= 0)
        {
            printf("\nError!Cannot receive data from sever!\n");
            close(client_sock);
            return 0;
        }
    }
    close(client_sock);
    return 0;
}
