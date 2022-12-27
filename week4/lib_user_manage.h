#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFF_SIZE 1024

void split_string(char *password, char *message){
    int i = 0, j = 0, k = 0;
    char number[100], character[100];
    memset(number, '\0', 100);
    memset(character, '\0', 100);
    number[0] = '@';
    character[0] = '@';
    // Split received string into character and number sub-string

    for (i = 0; i < strlen(password); i++)
    {
        if (isdigit(password[i]) != 0)
        {
            number[j] = password[i];
            j++;
        }
        else if (isalpha(password[i]) != 0)
        {
            character[k] = password[i];
            k++;
        }
        else {
            // If received string include non-digit or non-alphabet character,
            // return error message
            printf("Error: Invalid password\n");
            strcpy(message, "Error");
            return;
        }
    }

    strcpy(message, "OK");
    strcat(message, "-");
    strcat(message, number);
    strcat(message, "-");
    strcat(message, character);
    return;
}

// Define structure for user
typedef struct user_t
{
    char name[100];
    char password[100];
    int status;
    int false_login;
    int logged_in;
} s_user;

// Define structure for user linked list
typedef struct list_t
{
    s_user user;
    struct list *next;
} list;

// This function is used to copy user data from one structure to another
void copy_user(s_user *user, char *name, char *password, int status)
{
    strcpy(user->name, name);
    strcpy(user->password, password);
    user->status = status;
}

// This function is used to add new user to linked list
void add_user(list **head, s_user user)
{
    list *node = (list *)malloc(sizeof(list));
    copy_user(&node->user, user.name, user.password, user.status);
    node->user.false_login = 0;
    node->user.logged_in = 0;
    node->next = NULL;
    list *temp = *head;
    if (temp == NULL)
    {
        *head = node;
        node->next = NULL;
    }
    else
    {
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = node;
        node->next = NULL;
    }
}

// This function is used to check if user is already present in linked list
int check_exist(list *head, char *name)
{
    list *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->user.name, name) == 0)
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// This function is used to register new user
void register_user(list **head, char *name, char *password)
{
    s_user user;
    copy_user(&user, name, password, 1);
    if (check_exist(*head, user.name) == 0)
    {
        add_user(head, user);
        printf("\nUser registered successfully");
    }
    else
    {
        printf("\nUsername already exists");
    }
}

// This function is used to login user
void login(list *head, char *name, char *password, char *message)
{
    list *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->user.name, name) == 0)
        {
            if (strcmp(temp->user.password, password) == 0)
            {
                if (temp->user.status == 1)
                {
                    if (temp->user.logged_in == 0)
                    {
                        temp->user.logged_in = 1;
                        strcpy(message, "OK");
                        return;
                    }
                    else
                    {
                        strcpy(message, "User already logged in");
                        return;
                    }
                }
                else
                {
                    strcpy(message, "Account not ready");
                    return;
                }
            }
            else
            {
                temp->user.false_login++;
                if (temp->user.false_login == 3)
                {
                    temp->user.status = 0;
                    strcpy(message, "Account is blocked");
                    return;
                }
                strcpy(message, "Not OK");
                return;
            }
        }
        temp = temp->next;
    }
    strcpy(message, "User does not exist");
    return;
}

// This function is used to search user in linked list
char *search(list *head, char *name)
{
    list *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->user.name, name) == 0)
        {
            if (temp->user.status == 1)
            {
                return "OK";
            }
            else
            {
                return "Account not ready";
            }
        }
    }
    return "User does not exist";
}

// This function is used to logout user
void logout(list *head, char *name, char *message){
    list *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->user.name, name) == 0)
        {
            if (temp->user.logged_in == 1)
            {
                temp->user.logged_in = 0;
                strcpy(message, "Goodbye ");
                strcat(message, name);
                return;
            }
            else
            {
                strcpy(message, "User not logged in");
                return;
            }
        }
        temp = temp->next;
    }
    strcpy(message, "User does not exist");
    return;
}


// This function is used to change user password
void change_password(list *head, char *name, char *password, char *message){
    list *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->user.name, name) == 0)
        {
            if (temp->user.logged_in == 1)
            {
                split_string(password, message);
                if (strcmp(message, "Error") != 0){
                    strcpy(temp->user.password, password);
                }
                return;
            }
            else
            {
                strcpy(message, "User not logged in");
                return;
            }
        }
        temp = temp->next;
    }
    strcpy(message, "User does not exist");
    return;
}

// This function is used to read user data from file
void read_file(list **head)
{
    FILE *fp;
    char name[100], password[100];
    int status;
    fp = fopen("account.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening file");
        return;
    }
    while (fscanf(fp, "%s %s %d", name, password, &status) != EOF)
    {
        s_user user;
        copy_user(&user, name, password, status);
        add_user(head, user);
    }
    fclose(fp);
}

// This function is used to write user data to file
void write_file(list *head)
{
    FILE *fp;
    fp = fopen("account.txt", "w");
    if (fp == NULL)
    {
        printf("Error opening file");
        return;
    }
    list *temp = head;
    while (temp != NULL)
    {
        fprintf(fp, "%s %s %d\n", temp->user.name, temp->user.password, temp->user.status);
        temp = temp->next;
    }
    fclose(fp);
}

// This function is used to free user data from linked list
void free_list(list *head)
{
    list *temp = head;
    while (temp != NULL)
    {
        head = head->next;
        free(temp);
        temp = head;
    }
}



