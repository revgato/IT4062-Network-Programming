#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "user_manage.h"
#include "message.h"
#include "communicate.h"

int main(){
    int client_sock;
    char buff[BUFF_SIZE + 1];
    user client_info;
    msg message; 
    struct sockaddr_in server_addr;  /* server's address information */
    int msg_len, bytes_sent, bytes_received;

    // Step 1: Construct socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    // Step 3: Request to connect server
    if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
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
    if(bytes_received < 0)
        perror("\nError: ");
    else if (bytes_received == 0)
        printf("Connection closed.");
    
    if(message.type == LOGIN_FAIL){
        printf("Login fail\n");
    }else if(message.type == LOGIN_SUCCESS){
        printf("Login successfully\n");
    }
    printf("Received from server: %s\n", message.message);

    // Step 4: Close socket
    close(client_sock);
    return 0;
}