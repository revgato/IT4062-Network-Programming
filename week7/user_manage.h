#include <stdio.h>
#include <stdlib.h>

// Define structure of user

typedef struct user_t{
    char username[20];
    char password[20];
} user;

// Define list of user

typedef struct user_list_t{
    user *data;
    struct user_list_t *next;
} user_list;