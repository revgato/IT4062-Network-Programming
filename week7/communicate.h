#include <stdlib.h>

#define BUFF_SIZE 1024
#define SERVER_PORT 1234
#define BACKLOG 8
#define SERVER_ADDR "127.0.0.1"

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
