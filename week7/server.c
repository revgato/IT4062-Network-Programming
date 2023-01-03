#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
void send_all(conn_msg *message);

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
    while (1)
    {
        connfd = malloc(sizeof(int));
        if ((*connfd = accept(listenfd, (struct sockaddr *)client, &sin_size)) == -1)
            perror("\nError: ");

        printf("bao You got a connection from %s\n", inet_ntoa((*client).sin_addr)); /* Print client's IP */

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
    // client_list *temp = list_client;

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
    add_client(&list_client, connfd, client_info.username);

    // printf("Username: %s\n", client_info.username);
    // printf("Password: %s\n", client_info.password);

    memset(buff, '\0', BUFF_SIZE);
    strcpy(buff, "Hello ");
    strcat(buff, client_info.username);

    // Sent "Hello" to all client
    message = make_message_text(status, buff);
    // while (temp != NULL)
    bytes_sent = send(connfd, &message, sizeof(message), 0);
    if (bytes_sent < 0)
        perror("\nError: ");
    message.type = CONN_MESSAGE;
    send_all(&message);
    // send_log(connfd);
    // close(connfd);
}

void send_log(int connfd)
{
    int bytes_sent;
    message_list *temp = list_message;
    conn_msg conn_message;
    while (temp != NULL)
    {
        conn_message = make_message_chat(CHAT_MESSAGE, temp->msg);
        printf("Sent message: %s\n", conn_message.data.msg.message);
        bytes_sent = send(connfd, &conn_message, sizeof(conn_message), 0);
        if (bytes_sent < 0)
        {
            perror("\nError: ");
            break;
        }
        temp = temp->next;
    }
}

void send_all(conn_msg *message)
{
    client_list *temp = list_client;
    int bytes_sent;
    while (temp != NULL)
    {
        bytes_sent = send(temp->connfd, message, sizeof(*message), 0);
        if (bytes_sent < 0)
        {
            perror("\nError: ");
            break;
        }
        temp = temp->next;
    }
}