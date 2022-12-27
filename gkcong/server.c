#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SOCKET_1 5555
#define SOCKET_2 6666

#define BUFF_SIZE 1024

void string_process(char *buff, char *filename, char *port){
    // Check if buff string is match with prototype GET FILENAME PORT
    // If not, return error message
    char *token;
    token = strtok(buff, " ");
    if (strcmp(token, "GET") != 0){
        strcpy(buff, "Error\0");
        return;
    }
    token = strtok(NULL, " ");
    if (token == NULL){
        strcpy(buff, "Error\0");
        return;
    }
    strcpy(filename, token);
    token = strtok(NULL, " ");
    if (token == NULL){
        strcpy(buff, "Error\0");
        return;
    }
    strcpy(port, token);

    token = strtok(NULL, " ");
    if (token!=NULL){
        strcpy(buff, "Error\0");
        return;
    }
}

// Send file to TCP client
void send_file(char *filename, int connfd){
    FILE *fp;
    char buff[BUFF_SIZE];
    int bytes_send;

    fp = fopen(filename, "r");
    if (fp == NULL){
        strcpy(buff, "Error\0");
        bytes_send = send(connfd, buff, strlen(buff), 0);
        return;
    }
    while (fgets(buff, BUFF_SIZE, fp) != NULL){
        bytes_send = send(connfd, buff, strlen(buff), 0);
        if (bytes_send < 0){
            perror("\nError: ");
            return;
        }
    }
    bytes_send = send(connfd, "EOF", strlen("EOF"), 0);
    return;
}
    
void send_report(char *buff, int port_2){
    // Send report to TCP client
    int server_sock, client, connfd, sin_size;
    struct sockaddr_in server;
    int bytes_sent;

    // Construct a TCP socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        // Call socket() to create a socket
        perror("\nError: ");
        return;
    }

    // Bind address to socket
    server.sin_family = AF_INET;
    server.sin_port = htons(port_2);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&server.sin_zero, 8);

   if ((bind(server_sock, (struct sockaddr *)&server, sizeof(server))) == -1)
    {
        // Call bind() to bind address to socket
        perror("\nError: ");
        return;
    }

    // Listen request from client
    if (listen(server_sock, 5) == -1)
    {
        perror("\nError: ");
        return;
    }

    // Accept request from client
    sin_size = sizeof(struct sockaddr_in);
    if ((connfd = accept(server_sock, (struct sockaddr *)&client, &sin_size)) == -1)
    {
        perror("\nError: ");
        return;
    }

    // Send message to client
    bytes_sent = send(connfd, buff, strlen(buff), 0);
    close(server_sock);
}

int main(){
    struct sockaddr_in server, client_listen, client_send, client_report;
    int server_sock;
    int bytes_sent, bytes_received, sin_size;
    int port_1 = SOCKET_1;
    int port_2 = SOCKET_2;
    int port_0;
    char buff[BUFF_SIZE];
    char filename[BUFF_SIZE];
    char port[BUFF_SIZE];
    int listenfd, connfd;

    // Construct a UDP socket

    if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        // Call socket() to create a socket
        perror("\nError: ");
        return 0;
    }

    // Bind address to socket

    server.sin_family = AF_INET;
    server.sin_port = htons(port_1);
    server.sin_addr.s_addr = INADDR_ANY;
    printf("Port: %d\n", port_1);
    bzero(&server.sin_zero, 8);

    if ((bind(server_sock, (struct sockaddr *)&server, sizeof(server))) == -1)
    {
        // Call bind() to bind address to socket
        perror("\nError: ");
        return 0;
    }

    // Communicate with clients
    sin_size = sizeof(struct sockaddr_in);
    bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client_listen, &sin_size);
    if (bytes_received < 0)
    {
        perror("\nError: ");
        return 0;
    }

    printf("%s\n", buff);
    string_process(buff, filename, port);
    port_0 = atoi(port);

    // Close socket
    close(server_sock);

    // If buff is "Error", send error message to client
    if (strcmp(buff, "Error") == 0){
        send_report("INVALID COMMAND", port_2);
    }

    // Create TCP socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        // Call socket() to create a socket
        perror("\nError: ");
        return 0;
    }

    // Bind address to socket

    server.sin_family = AF_INET;
    server.sin_port = htons(port_0);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((bind(server_sock, (struct sockaddr *)&server, sizeof(server))) == -1)
    {
        // Call bind() to bind address to socket
        perror("\nError: ");
        return 0;
    }

    // Listen request from client
    if (listen(server_sock, 5) == -1)
    {
        perror("\nError: ");
        return 0;
    }

    // Accept request from client
    sin_size = sizeof(struct sockaddr_in);
    if ((connfd = accept(server_sock, (struct sockaddr *)&client_send, &sin_size)) == -1)
    {
        perror("\nError: ");
        return 0;
    }

    // Send file to client
    send_file(filename, connfd);

    // Send "DONE" message to client 
    send_report("DONE", port_2);

    // Close connection
    close(connfd);
    return 0;
}