#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define structure of user
typedef struct user_t{
    char username[20];
    char password[20];
} user;

// Define list of user
typedef struct user_list_t{
    user usr;
    struct user_list_t *next;
} user_list;

// Read user from file
void read_user_data(user_list **head){
    FILE *f = fopen("taikhoan.txt", "r");
    if(f == NULL){
        printf("Cannot open file taikhoan.txt\n");
        exit(1);
    }
    user_list *temp = *head;
    while(!feof(f)){
        user_list *new_node = (user_list *)malloc(sizeof(user_list));
        new_node->next = NULL;
        fscanf(f, "%s %s", new_node->usr.username, new_node->usr.password);
        if(*head == NULL){
            *head = new_node;
            temp = *head;
        }
        else{
            while(temp->next != NULL){
                temp = temp->next;
            }
            temp->next = new_node;
            temp = temp->next;
        }
    }
    fclose(f);
}

// Login validation
int login(user_list *head, char *username, char *password){
    user_list *temp = head;
    while(temp != NULL){
        if(strcmp(temp->usr.username, username) == 0 && strcmp(temp->usr.password, password) == 0){
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// Traverse user list and print user info 
void traverse_list(user_list *head){
    user_list *temp = head;
    while(temp != NULL){
        printf("Username: %s\n", temp->usr.username);
        printf("Password: %s\n", temp->usr.password);
        temp = temp->next;
    }
}