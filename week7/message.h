#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define structure of message
typedef struct message_t
{
    char username[20];
    char message[100];
} message;

// Define linked list of message
typedef struct message_list_t
{
    message msg;
    struct message_list_t *next;
} message_list;

// Read all message from file
void read_message_data(message_list **head)
{
    FILE *f = fopen("groupchat.txt", "r");
    if (f == NULL)
    {
        printf("Cannot open file groupchat.txt\n");
        exit(1);
    }
    message_list *temp = *head;
    while (!feof(f))
    {
        message_list *new_node = (message_list *)malloc(sizeof(message_list));
        new_node->next = NULL;
        // Read line until meet : character. Username and message split by : character
        fscanf(f, "%[^:]%*c%[^\n]%*c", new_node->msg.username, new_node->msg.message);
        printf("%s %s\n", new_node->msg.username, new_node->msg.message);
        if (*head == NULL)
        {
            *head = new_node;
            temp = *head;
        }
        else
        {
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = new_node;
            temp = temp->next;
        }
    }
    fclose(f);
}

void print_message(message msg)
{
    printf("[%s]: %s\n", msg.username, msg.message);
}

void traverse_message(message_list *head)
{
    message_list *temp = head;
    while (temp != NULL)
    {
        print_message(temp->msg);
        temp = temp->next;
    }
}