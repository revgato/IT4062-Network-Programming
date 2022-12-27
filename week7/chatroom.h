#include <stdio.h>
#include <stdlib.h>

// Define structure of message
typedef struct message_t
{
    char username[50];
    char message[100];
    // char time[50];
} message;

// Define structure of message linked list
typedef struct list_t
{
    message msg;
    struct list_t *next;
} list;

// This function will copy the message to the message structure
void copy_message(message *msg, char *username, char *message, char *time){
    strcpy(msg->username, username);
    strcpy(msg->message, message);
    // strcpy(msg->time, time);
}
void add_message(list **head, message msg){
    list *node = (list *)malloc(sizeof(list));
    copy_message(&node->msg, msg.username, msg.message);
    list *temp = *head;
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
