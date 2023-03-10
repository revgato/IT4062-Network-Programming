#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "user_manage.h"
#include "message.h"
#include "communicate.h"

// Load user data from taikhoan.txt file
user_list *list_user;
message_list *list_message;
client_list *list_client;

// Handle client task
void *client_handle(void *);

// Send all log message to client
void send_log(int connfd);

// Send message to all client
void send_all(conn_msg *message, int connfd);

// Catch signal when server terminated
void catch_ctrl_c(int sig);

int main()
{
    int listenfd, *connfd;
    struct sockaddr_in server;  /* server's address information */
    struct sockaddr_in *client; /* server's address information */
    int sin_size;
    pthread_t tid;

    read_user_data(&list_user);
    read_message_data(&list_message);
    // traverse_user(list_user);                // Read user data OK
    // traverse_message(list_message);          // Read message OK

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { /* calls sockets() */
        perror("\nError: ");
        return 0;
    }
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

    if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("\nError: ");
        return 0;
    }

    if (listen(listenfd, BACKLOG) == -1)
    {
        perror("\nError: ");
        return 0;
    }

    sin_size = sizeof(struct sockaddr_in);
    client = malloc(sin_size);
    // signal(SIGINT, catch_ctrl_c);
    while (1)
    {
        connfd = malloc(sizeof(int));
        if ((*connfd = accept(listenfd, (struct sockaddr *)client, &sin_size)) == -1)
            perror("\nError: ");

        printf("You got a connection from %s\n", inet_ntoa((*client).sin_addr)); /* Print client's IP */

        // For each client, spawns a thread, and the thread handles the new client
        pthread_create(&tid, NULL, &client_handle, connfd);
    }

    close(listenfd);
    return 0;
}

void *client_handle(void *arg)
{
    int connfd;
    int bytes_sent, bytes_received;
    char buff[BUFF_SIZE + 1];
    user client_info;
    conn_msg message;
    conn_msg_type status;
    client_list *client_temp;

    connfd = *((int *)arg);
    free(arg);
    pthread_detach(pthread_self());
    bytes_received = recv(connfd, &client_info, sizeof(client_info), 0); // blocking
    if (bytes_received < 0)
        perror("\nError: ");
    else if (bytes_received == 0)
        printf("Connection closed.");

    if (login(list_user, client_info.username, client_info.password))
    {
        status = LOGIN_SUCCESS;
    }
    else
    {
        status = LOGIN_FAIL;
    }
    // If LOGIN_SUCCESS
    if (status == LOGIN_SUCCESS)
    {
        add_client(&list_client, connfd, client_info.username);

        memset(buff, '\0', BUFF_SIZE);
        strcpy(buff, client_info.username);
        strcat(buff, " has joined this room");

        // Sent "Hello" to all client
        message = make_message_text(status, buff);
        bytes_sent = send(connfd, &message, sizeof(message), 0);
        if (bytes_sent < 0)
            perror("\nError: ");
        message.type = CONN_MESSAGE;
        send_all(&message, connfd);
        send_log(connfd);
        // Receive chat message from client
        while (1)
        {
            bytes_received = recv(connfd, &message, sizeof(message), 0);
            if (bytes_received < 0){
                perror("\nError: ");
                break;
            }
            else if (bytes_received == 0)
            {
                printf("Connection closed.");
                break;
            }
            send_all(&message, connfd);
            update_log_file(message.data.msg);
            add_message(&list_message, message.data.msg);
        }
        client_temp = find_client(list_client, connfd);
        delete_client(&list_client, client_temp);

        close(connfd);
    }

    // Send login fail message to client
    message = make_message_text(LOGIN_FAIL, buff);
    bytes_sent = send(connfd, &message, sizeof(message), 0);

    close(connfd);
}

// Send chat history to joined client
void send_log(int connfd)
{
    int bytes_sent;
    message_list *temp = list_message;
    conn_msg conn_message;
    while (temp != NULL)
    {
        conn_message = make_message_chat(temp->msg);
        // printf("Sent message: %s\n", conn_message.data.msg.message);
        bytes_sent = send(connfd, &conn_message, sizeof(conn_message), 0);
        if (bytes_sent < 0)
        {
            perror("\nError: ");
            break;
        }
        temp = temp->next;
    }
}

// Send message contain to all client
void send_all(conn_msg *message, int connfd)
{
    client_list *temp = list_client;
    int bytes_sent;
    while (temp != NULL)
    {
        // If this node is client itself, skip
        if(temp->connfd == connfd){
            temp = temp->next;
            continue;
        }
        bytes_sent = send(temp->connfd, message, sizeof(*message), 0);
        if (bytes_sent < 0)
        {
            perror("\nError: ");
            break;
        }
        temp = temp->next;
    }
}

// void catch_ctrl_c(int sig){
//     int bytes_sent;
//     conn_msg conn_message;
//     make_message_text(CONN_MESSAGE, "Server terminated\n");
//     send_all(&conn_message);
//     exit(0);
// }