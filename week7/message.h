#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define structure of message
typedef struct message_t{
    char username[20];
    char message[100];
}message;

// Define linked list of message
typedef struct list_message_t{
    message msg;
    struct list_message_t *next;
}list_message;

