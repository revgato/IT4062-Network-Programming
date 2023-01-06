#ifndef COMMUNICATE_H
#define COMMMUNICATE_H

#include <stdlib.h>
#include "hardware.h"

// #define SERVER_PORT 5500
#define BUFF_SIZE 300
#define BACKLOG 8
// #define SERVER_ADDR "127.0.0.1"

// Define communicate message
typedef enum conn_msg_type_t
{
    HARDWARE_INFORMATION,
    CONN_MESSAGE
} conn_msg_type;

// Define data of communicate message
typedef union conn_data_t{
    hardware hwr;
    char text[BUFF_SIZE];
} conn_data;
// Define structure of network message
typedef struct conn_msg_t
{
    conn_msg_type type;
    conn_data data;
} conn_msg;

conn_msg make_message_hwr(hardware hwr){
    conn_msg conn_message;
    copy_hardware(&(conn_message.data.hwr), hwr);
    conn_message.type = HARDWARE_INFORMATION;
    return conn_message;
}

conn_msg make_message_text(char *text){
    conn_msg conn_message;
    strcpy(conn_message.data.text, text);
    conn_message.type = CONN_MESSAGE;
    return conn_message;
}

#endif


