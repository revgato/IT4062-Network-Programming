#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>

#define BUFF_SIZE 1024

typedef struct msg_t
{
  enum msg_type
  {
    LOGIN,
    LOGIN_SUCCESS,
    FOUND_USER,
    FOUND_USER_BUT_LOCKED,
    NOT_FOUND_USER,
    PASS_INVALID
  } type;
  char value[BUFF_SIZE];
} Msg;

char prompt[50];

// read server response
void reader(void *var)
{
  char buf[BUFF_SIZE];
  int bytes_received;
  int connID = (int)var;

  while ((bytes_received = recv(connID, buf, BUFF_SIZE, 0)) > 0)
  {
    if (strlen(buf) != 0)
    {
      buf[bytes_received] = '\0';
      printf("\n%s\n%s", buf, prompt);
      fflush(stdout);
    }
    else
    {
      printf("\n%s", prompt);
      fflush(stdout);
    }
  }
}

int main(int argc, char const *argv[])
{
  pthread_t tid;

  if (argc != 5)
  {
    printf("Usage: ./client IPAddress PortNumber username password\n\n");
    return 0;
  }

  int client_sock;
  char buff[BUFF_SIZE + 1], chat_text[BUFF_SIZE + 1];
  struct sockaddr_in server_addr; /* server's address information */
  int msg_len, bytes_sent, bytes_received;

  // Step 1: Construct socket
  client_sock = socket(AF_INET, SOCK_STREAM, 0);

  // Step 2: Specify server address
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[2]));
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);

  // Step 3: Request to connect server
  if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
  {
    printf("\nError!Can not connect to sever! Client exit imediately! ");
    return 0;
  }

  // Step 4: Communicate with server
  Msg msg;

  // get username and password from command line
  msg.type = LOGIN;
  strcpy(msg.value, argv[3]);
  strcat(msg.value, " ");
  strcat(msg.value, argv[4]);

  // send username to server
  bytes_sent = send(client_sock, &msg, sizeof(msg), 0);
  if (bytes_sent < 0)
  {
    printf("\nConnection closed!\n");
    return 0;
  }

  bytes_received = recv(client_sock, &msg, sizeof(msg), 0);
  if (bytes_received < 0)
  {
    perror("Error: ");
    return 0;
  }

  switch (msg.type)
  {
  case LOGIN_SUCCESS:
    printf("Login successfully! Enter groupchat...\n");

    sprintf(prompt, "[%s]> ", argv[3]);

    // receive group chat history from server
    bytes_received = recv(client_sock, &msg, sizeof(msg), 0);
    if (bytes_received < 0)
    {
      printf("\nConnection closed!\n");
      break;
    }
    printf("%s", msg.value);

    pthread_create(&tid, NULL, reader, (void *)client_sock);

    // chat loop
    while (1)
    {
      printf("%s", prompt);
      // get chat text from user
      if ((fgets(chat_text, BUFF_SIZE, stdin) == NULL) && ferror(stdin))
      {
        perror("fgets error");
        close(client_sock);
        exit(1);
      }
      // remove the trailing newline
      chat_text[strlen(chat_text) - 1] = '\0';

      // send chat text to server
      if (send(client_sock, chat_text, strlen(chat_text), 0) <= 0)
      {
        perror("not able to send the data");
        close(client_sock);
        exit(1);
      }
    }
    break;
  case FOUND_USER_BUT_LOCKED:
    printf("User is locked, now exitting...\n");
    close(client_sock);
    break;
  case NOT_FOUND_USER:
    printf("User not found, now exitting...\n");
    close(client_sock);
    break;
  case PASS_INVALID:
    printf("Password is invalid, now exitting...\n");
    close(client_sock);
    break;
  }

  return 0;
}
