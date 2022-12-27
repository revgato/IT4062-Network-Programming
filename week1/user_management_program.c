#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define structure for user
typedef struct user_t {
    char name[100];
    char password[100];
    int status;
    int false_login;
    int logged_in;
}s_user;

// Define structure for user linked list
typedef struct list_t {
    s_user user;
    struct list *next;
}list;

// This function is used to copy user data from one structure to another
void copy_user(s_user *user, char *name, char *password, int status) {
    strcpy(user->name, name);
    strcpy(user->password, password);
    user->status = status;
}

// This function is used to add new user to linked list
void add_user(list **head, s_user user) {
    list *node = (list *)malloc(sizeof(list));
    copy_user(&node->user, user.name, user.password, user.status);
    node->user.false_login = 0;
    node->user.logged_in = 0;
    node->next = NULL;
    list *temp = *head;
    if (temp == NULL) {
        *head = node;
        node->next = NULL;
    } else {
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = node;
        node->next = NULL;
    }
}

// This function is used to check if user is already present in linked list
int check_exist(list *head, char *name) {
    list *temp = head;
    while (temp != NULL) {
        if (strcmp(temp->user.name, name) == 0) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// This function is used to register new user
void register_user(list **head) {
    s_user user;
    printf("Username: ");
    scanf("%s", user.name);

    if(check_exist(*head, user.name) == 1) {
        printf("Account existed\n");
    } else {
        printf("Password: ");
        scanf("%s", user.password);
        user.status = 1;
        add_user(head, user);
        printf("Successful registration\n");
    }
}

// This function is used to login user
void login(list *head) {
    char name[100];
    char password[100];
    printf("Username: ");
    scanf("%s", name);
    list *temp = head;
    while (temp != NULL) {
        if (strcmp(temp->user.name, name) == 0) {
            if (temp->user.status == 0) {
                printf("Account is blocked\n");
                return;
            }
            printf("Password: ");
            scanf("%s", password);
            if (strcmp(temp->user.password, password) == 0) {
                printf("Hello %s\n", temp->user.name);
                temp->user.logged_in = 1;
                return;
            } else {
                temp->user.false_login++;
                if (temp->user.false_login == 3) {
                    temp->user.status = 0;
                    printf("Password is incorrect. Account is blocked\n");
                    return;
                }
                printf("Password is incorrect\n");
                return;
            }
        }
        temp = temp->next;
    }
    printf("Cannot find account\n");
}

// This function is used to search user in linked list
void search(list *head) {
    char name[100];
    printf("Username: ");
    scanf("%s", name);
    list *temp = head;
    while (temp != NULL) {
        if (strcmp(temp->user.name, name) == 0) {
            if(temp->user.status == 1) {
                printf("Account is active\n");
                return;
            } else {
                printf("Account is blocked\n");
                return;
            }
        }
        temp = temp->next;
    }
    printf("Cannot find account\n");
}

// This function is used to logout user
void logout(list *head) {
    char name[100];
    printf("Username: ");
    scanf("%s", name);
    list *temp = head;
    while (temp != NULL) {
        if (strcmp(temp->user.name, name) == 0) {
            if (temp->user.logged_in == 1) {
                printf("Goodbye %s\n", temp->user.name);
                temp->user.logged_in = 0;
                return;
            } else {
                printf("Account is not sign in\n");
                return;
            }
            return;
        }
        temp = temp->next;
    }
    printf("Cannot find account\n");
}

// This function is used to print user information
void print_user(s_user user){
    printf("%s %s %d\n", user.name, user.password, user.status);
    return;
}

// This function is used to print all user in linked list
void print_all_users(list *head) {
    list *temp = head;
    while (temp != NULL) {
        print_user(temp->user);
        temp = temp->next;
    }
}

// This function is used to read user data from file
void read_file(list **head) {
    FILE *fp = fopen("account.txt", "r");
    if(fp == NULL) {
        printf("File not found");
        return;
    }

    s_user user;
    while(fscanf(fp, "%s %s %d", user.name, user.password, &user.status) != EOF) {
        add_user(head, user);
    }
}

// This function is used to write user data to file
void write_file(list *head) {
    FILE *fp = fopen("account.txt", "w");
    if(fp == NULL) {
        printf("File not found");
        return;
    }

    list *temp = head;
    while(temp != NULL) {
        fprintf(fp, "%s %s %d\n", temp->user.name, temp->user.password, temp->user.status);
        temp = temp->next;
    }
}



// This function is used to free user linked list
void free_list(list *head) {
    list *temp = head;
    while (temp != NULL) {
        list *temp2 = temp;
        temp = temp->next;
        free(temp2);
    }
}

// This function is used to print menu
int menu(){
    int choice;
    printf("\n\n\n\nUSER MANAGEMENT PROGRAM\n"
        "---------------------------\n"
        "1. Register\n"
        "2. Sign in\n"
        "3. Search\n"
        "4. Sign out\n"
        "Your choice (1-4, other to quit)\n");
    scanf("%d", &choice);
    return choice;
}


int main(){
    
    int choice;

    list *head = NULL;
    read_file(&head);
    while(1){
        choice = menu();
        switch(choice){
            case 1:
                register_user(&head);
                break;
            case 2:
                login(head);
                break;
            case 3:
                search(head);
                break;
            case 4:
                logout(head);
                break;
            default:
                write_file(head);
                free_list(head);
                return 0;
        }
    }
}