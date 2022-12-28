#include <stdlib.h>

#define BUFF_SIZE 1024
typedef struct msg_t
{
    enum msg_type
    {
        LOGIN,
        LOGIN_SUCCESS,
        LOGIN_FAIL
    } type;

    char message[BUFF_SIZE];
}msg;

typedef struct client_list_t{
    char username[20];
    int conn_fd;
    struct client_list_t *next;
}client_list;

client_list *new_client(){
    client_list *node = (client_list *)malloc(sizeof(client_list));
    node->next = NULL;
    return node;
}

void add_client(client_list **head, int conn_fd, char* username){
    client_list *node = new_client();
    node->conn_fd = conn_fd;
    if(*head == NULL){
        *head = node;
    }else{
        client_list *temp = *head;

    }
}