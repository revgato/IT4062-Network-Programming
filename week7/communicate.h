#ifndef COMMUNICATE_H
#define COMMMUNICATE_H

#include <stdlib.h>
#include "message.h"

#define SERVER_PORT 5512
#define BUFF_SIZE 1024
#define BACKLOG 8
#define SERVER_ADDR "127.0.0.1"

// Define communicate message
typedef enum conn_msg_type_t
{
    LOGIN,
    LOGIN_SUCCESS,
    CHAT_MESSAGE,
    CONN_MESSAGE,
    LOGIN_FAIL
} conn_msg_type;

// Define data of communicate message
typedef union conn_data_t{
    message msg;
    char text[BUFF_SIZE];
} conn_data;
// Define structure of network message
typedef struct conn_msg_t
{
    conn_msg_type type;
    conn_data data;
} conn_msg;

conn_msg make_message_text(conn_msg_type status, char *buff){
    conn_msg message;
    strcpy(message.data.text, buff);
    message.type = status;
    return message;
}

conn_msg make_message_chat(message msg){
    conn_msg conn_message;
    copy_chat_message(&(conn_message.data.msg), msg);
    conn_message.type = CHAT_MESSAGE;
    return conn_message;
}

// Define client's linked list
typedef struct client_list_t
{
    char username[20];
    int connfd;
    struct client_list_t *next;
} client_list;

// To make this library more general, should write your own comparison function
int compare_client(client_list *client1, client_list *client2){
    return client1->connfd == client2->connfd;
}

// Generate new client node
client_list *new_client()
{
    client_list *node = (client_list *)malloc(sizeof(client_list));
    node->next = NULL;
    return node;
}

// Add client to linked list
void add_client(client_list **head, int connfd, char *username)
{
    client_list *node = new_client();
    node->connfd = connfd;
    strcpy(node->username, username);
    if (*head == NULL)
    {
        *head = node;
    }
    else
    {
        client_list *temp = *head;

        // Traverse to the end of linked list
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = node;
    }
}

// Delete client function

void delete_client(client_list **head, client_list *client)
{
    client_list *temp = *head;
    client_list *temp_prev = NULL;

    while (temp != NULL)
    {
        if (compare_client(temp, client))
        {
            // If connfd is in head node
            if (temp_prev == NULL)
            {
                *head = temp->next;
            }
            else
            {
                temp_prev->next = temp->next;
            }
            free(temp);
            return;
        }

        temp_prev = temp;
        temp = temp->next;
    }
}

// Find client with connfd
client_list *find_client(client_list *head, int connfd){
    client_list *temp = head;
    while(temp != NULL){
        if(temp->connfd == connfd){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

#endif
