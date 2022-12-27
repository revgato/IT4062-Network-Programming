#include <stdio.h>
#include <stdlib.h>

// Define structure of message
typedef struct message_t
{
    char username[50];
    char message[100];
} message;

// Define structure of message linked list
typedef struct message_list_t
{
    message msg;
    struct list_t *next;
} message_list;

// This function will copy the message to the message structure
void copy_message(message *msg, char *username, char *message){
    strcpy(msg->username, username);
    strcpy(msg->message, message);
    // strcpy(msg->time, time);
}
void add_message(message_list **head, message msg){
    message_list *node = (message_list *)malloc(sizeof(message_list));
    copy_message(&node->msg, msg.username, msg.message);
    message_list *temp = *head;
    if(temp == NULL){
        *head = node;
        node->next = NULL;
    }
    else{
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = node;
        node->next = NULL;
    }
}
