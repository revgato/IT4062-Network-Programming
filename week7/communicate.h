#ifndef COMMUNICATE_H
#define COMMMUNICATE_H

#include <stdlib.h>
#include "message.h"

#define SERVER_PORT 1253
#define BUFF_SIZE 1024
#define BACKLOG 8
#define SERVER_ADDR "127.0.0.1"

// Define communicate message
typedef enum conn_msg_type_t
{
    LOGIN,
    LOGIN_SUCCESS,
    CHAT_MESSAGE,
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

conn_msg make_message_chat(conn_msg_type status, message msg){
    conn_msg conn_message;
    copy_chat_message(&(conn_message.data.msg), msg);
    conn_message.type = status;
    return conn_message;
}

// Define element's linked list
typedef struct element_list_t
{
    char username[20];
    int connfd;
    struct element_list_t *next;
} element_list;

// To make this library more general, should write your own comparison function
int compare_element(element_list *element1, element_list *element2){
    return element1->connfd == element2->connfd;
}

// Generate new element node
element_list *new_element()
{
    element_list *node = (element_list *)malloc(sizeof(element_list));
    node->next = NULL;
    return node;
}

// Add element to linked list
void add_element(element_list **head, int connfd, char *username)
{
    element_list *node = new_element();
    node->connfd = connfd;
    if (*head == NULL)
    {
        *head = node;
    }
    else
    {
        element_list *temp = *head;

        // Traverse to the end of linked list
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = node;
    }
}

// Delete element function

void delete_element(element_list **head, element_list *element)
{
    element_list *temp = *head;
    element_list *temp_prev = NULL;

    while (temp->next != NULL)
    {
        if (compare_element(temp, element))
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

// Find element
element_list *find_element(element_list *head, int connfd){
    element_list *temp = head;
    while(temp->next != NULL){
        if(temp->connfd == connfd){
            return temp;
        }
    }
    return NULL;
}

#endif
