#include <stdlib.h>

#define BUFF_SIZE 1024

// Define structure of network message
typedef struct msg_t
{
    enum msg_type
    {
        LOGIN,
        LOGIN_SUCCESS,
        LOGIN_FAIL
    } type;

    char message[BUFF_SIZE];
} msg;

// Define client's linked list
typedef struct client_list_t
{
    char username[20];
    int conn_fd;
    struct client_list_t *next;
} client_list;

// Generate new client node
client_list *new_client()
{
    client_list *node = (client_list *)malloc(sizeof(client_list));
    node->next = NULL;
    return node;
}

// Add client to linked list
void add_client(client_list **head, int conn_fd, char *username)
{
    client_list *node = new_client();
    node->conn_fd = conn_fd;
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

void delete_client(client_list **head, int conn_fd)
{
    client_list *temp = *head;
    client_list *temp_prev = NULL;

    while (temp->next != NULL)
    {
        if (temp->conn_fd == conn_fd)
        {
            // If conn_fd is in head node
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