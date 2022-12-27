#include <stdio.h>
#include <stdlib.h>
// Define structure of user
typedef struct user_t
{
    char username[50];
    char password[50];
} user;

// Define structure of user linked list
typedef struct user_list_t
{
    user usr;
    struct user_list_t *next;
} user_list;


// This function read the user data from the file and store it in the linked list
void read_user_data(user_list **head){
    FILE *fp = fopen("taikhoan.txt", "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }
    user_list *temp = *head;
    while(!feof(fp)){
        user_list *node = (user_list *)malloc(sizeof(user_list));
        fscanf(fp, "%s %s", node->usr.username, node->usr.password);
        // printf("Usrname: %s, Password: %s.\n", node->usr.username, node->usr.password);
        if(temp == NULL){
            *head = node;
            temp = *head;
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
    fclose(fp);
}


// Print all user data
void print_user_data(user_list *head){
    user_list *temp = head;
    while(temp != NULL){
        printf("Username: %s, Password: %s.\n", temp->usr.username, temp->usr.password);
        temp = temp->next;
    }
}

// This fuction check if the username and password is correct
int login(user_list *head, char *username, char *password){
    user_list *temp = head;
    while(temp != NULL){
        if(strcmp(temp->usr.username, username) == 0){
            if(strcmp(temp->usr.password, password) == 0){
                return 1;
            }
            else{
                return 0;
            }
        }
        temp = temp->next;
    }
    return 0;
}
