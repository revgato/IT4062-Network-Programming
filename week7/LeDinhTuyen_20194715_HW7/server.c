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

#define PORT 5500
#define BACKLOG 8
#define BUFF_SIZE 1024

char group_chat_history[BUFF_SIZE];

typedef struct node_t
{
  char username[BUFF_SIZE];
  char password[BUFF_SIZE];
  int status;
  struct node_t *next;
} Node;

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

typedef struct _client
{
  char login_account[BUFF_SIZE];
  int conn_fd;
  struct _client *next;
} Client;
Client *head_client = NULL;

Client *new_client()
{
  Client *new = (Client *)malloc(sizeof(Client));
  new->next = NULL;
  return new;
}

void add_client(int conn_fd)
{
  Client *new = new_client();
  new->conn_fd = conn_fd;
  if (head_client == NULL)
    head_client = new; // if linked list is empty
  else
  {
    Client *tmp = head_client; // assign head to p
    while (tmp->next != NULL)
      tmp = tmp->next; // traverse the list until the last node
    tmp->next = new;   // Point the previous last node to the new node created.
  }
}

void delete_client(int conn_fd)
{
  Client *tmp = head_client;
  Client *prev = NULL;
  while (tmp != NULL)
  {
    if (tmp->conn_fd == conn_fd)
    {
      if (prev == NULL)
        head_client = tmp->next;
      else
        prev->next = tmp->next;
      free(tmp);
      break;
    }
    prev = tmp;
    tmp = tmp->next;
  }
}

Node *head;

/* Handler process signal*/
void sig_chld(int signo)
{
  pid_t pid;
  int stat;

  /* Wait the child process terminate */
  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("Child %d terminated\n", pid);
}

int load_account(char *filename)
{
  int status, count = 0; // number of accounts
  FILE *f;
  char username[BUFF_SIZE], password[BUFF_SIZE];
  Node *current;
  head = current = NULL;

  // open file data
  if ((f = fopen(filename, "r")) == NULL)
  {
    printf("Cannot open file!\n");
    return 0;
  }

  // load accounts to linked list
  while (fscanf(f, "%s %s %d\n", username, password, &status) != EOF)
  {
    // create new node
    Node *node = malloc(sizeof(Node));
    strcpy(node->username, username);
    strcpy(node->password, password);
    node->status = status;

    // add node to list
    if (head == NULL)
      current = head = node;
    else
      current = current->next = node;
    count++;
  }

  fclose(f);
  return 1;
}

Node *find_node(char *username)
{
  Node *current = head;
  while (current != NULL)
  {
    if (strcmp(current->username, username) == 0)
      return current;
    current = current->next;
  }
  return NULL;
}

void save_account(char *filename)
{
  FILE *f;
  f = fopen(filename, "w");
  Node *current;
  for (current = head; current; current = current->next)
    fprintf(f, "%s %s %d\n", current->username, current->password, current->status);
  fclose(f);
}

int load_chat(char *filename)
{
  FILE *f;
  char buff[BUFF_SIZE];
  int count = 0;
  if ((f = fopen(filename, "r")) == NULL)
  {
    printf("Cannot open file %s\n", filename);
    return 0;
  }

  while (fgets(buff, BUFF_SIZE, f) != NULL)
  {
    strcat(group_chat_history, buff);
  }

  return 1;
}

int save_chat(char *filename)
{
  FILE *f;
  f = fopen(filename, "w");
  fprintf(f, "%s", group_chat_history);
  fclose(f);
}

/*
 * Receive and echo message to client
 * [IN] sockfd: socket descriptor that connects to client
 */
void *echo(void *conn_fd)
{
  pthread_detach(pthread_self());

  int bytes_received, bytes_sent;
  char username[BUFF_SIZE], password[BUFF_SIZE], chat_text[BUFF_SIZE], buf[BUFF_SIZE];
  Msg msg;
  Node *found = NULL;
  char prompt[50];
  int sockfd = *(int *)conn_fd;
  Client *cli = head_client;

  while (cli->conn_fd != sockfd && cli != NULL)
    cli = cli->next;

  bytes_received = recv(sockfd, &msg, sizeof(msg), 0); // blocking
  if (bytes_received <= 0)
  {
    printf("[%s]: Disconnected!\n", username);
    close(sockfd);
    delete_client(sockfd);
    pthread_exit(NULL);
    return NULL;
  }

  for (int i = 0; i < strlen(msg.value); i++)
  {
    if (msg.value[i] == ' ')
    {
      strncpy(username, msg.value, i);
      username[i] = '\0';
      strcpy(password, msg.value + i + 1);
      break;
    }
  }

  strcpy(cli->login_account, username);

  found = find_node(username);
  if (found != NULL)
  {
    if (found->status == 1)
      msg.type = FOUND_USER; // username found
    else
      msg.type = FOUND_USER_BUT_LOCKED; // username found but has been locked
  }
  else
    msg.type = NOT_FOUND_USER; // username not found

  // send notification to client
  if (msg.type != FOUND_USER)
  {
    bytes_sent = send(sockfd, &msg, sizeof(msg), 0); /* echo to the client */
    if (bytes_sent < 0)
    {
      perror("\nError: ");
      printf("[%s]: Disconnected!\n", username);
      close(sockfd);
      delete_client(sockfd);
      pthread_exit(NULL);
      return NULL;
    }
  }

  // validate password
  if (strcmp(found->password, password) == 0)
    msg.type = LOGIN_SUCCESS; // pass valid
  else
    msg.type = PASS_INVALID; // pass invalid

  // send notification to client
  bytes_sent = send(sockfd, &msg, sizeof(msg), 0); /* echo to the client */
  if (bytes_sent <= 0)
  {
    perror("\nError: ");
    printf("[%s]: Disconnected!\n", username);
    close(sockfd);
    delete_client(sockfd);
    pthread_exit(NULL);
    return NULL;
  }

  if (msg.type == LOGIN_SUCCESS)
  {
    sprintf(prompt, "%s: ", username);

    printf("Hello %s\n", username);

    strcpy(msg.value, group_chat_history);

    bytes_sent = send(sockfd, &msg, sizeof(msg), 0); /* echo to the client */
    if (bytes_sent <= 0)
    {
      perror("\nError: ");
      printf("[%s]: Disconnected!\n", username);
      close(sockfd);
      delete_client(sockfd);
      pthread_exit(NULL);
      return NULL;
    }
  }


  while ((bytes_received = recv(sockfd, buf, BUFF_SIZE, 0)) > 0)
  {
    buf[bytes_received] = '\0';
    sprintf(chat_text, "%s%s", prompt, buf);
    printf("%s\n", chat_text);
    strcat(group_chat_history, chat_text);
    strcat(group_chat_history, "\n");
    save_chat("groupchat.txt");


    Client *current = head_client;
    while (current != NULL)
    {
      if (current->conn_fd != sockfd)
      {
        bytes_sent = send(current->conn_fd, chat_text, strlen(chat_text), 0); /* echo to the client */
        if (bytes_sent <= 0)
        {
          perror("\nError: ");
          printf("[%s]: Disconnected!\n", username);
          close(sockfd);
          delete_client(sockfd);
          pthread_exit(NULL);
          return NULL;
        }
      }
      current = current->next;
    }

    send(sockfd, "", strlen(""), 0);
  }

  if (bytes_received <= 0)
  {
    printf("[%s]: Disconnected!\n", username);
    close(sockfd);
    delete_client(sockfd);
  }

  pthread_exit(NULL);
  return NULL;
}

void catch_ctrl_c_and_exit(int sig)
{
  char mesg[] = "\nServer is closing...\n";
  while (head_client != NULL)
  {
    if (send(head_client->conn_fd, mesg, strlen(mesg), 0) < 0)
    {
      perror("Send error");
      delete_client(head_client->conn_fd);
    }
    printf("\nClose socketfd: %d\n", head_client->conn_fd);
    delete_client(head_client->conn_fd);
  }
  printf("\nBye\n");
  exit(0);
}

int main(int argc, char const *argv[])
{
  // load data from file

  int listen_sock, conn_sock; /* file descriptors */
  struct sockaddr_in server;  /* server's address information */
  struct sockaddr_in client;  /* client's address information */
  pid_t pid;
  pthread_t tid;
  int sin_size;
  int optval = 1;

  if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  { /* calls socket() */
    printf("socket() error\n");
    return 0;
  }

  bzero(&server, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

  // setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval,sizeof(int));

  if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
  {
    perror("\nError: ");
    return 0;
  }

  if (listen(listen_sock, BACKLOG) == -1)
  {
    perror("\nError: ");
    return 0;
  }

  printf("Server is running at port %d\n", PORT);

  if (load_account("taikhoan.txt") == 0)
  {
    printf("Cannot load account\n");
    exit(1);
  }
  else
  {
    printf("Load account successfully\n");
  }

  // load chat history
  strcpy(group_chat_history, "");
  if (load_chat("groupchat.txt") == 0)
  {
    printf("Cannot load groupchat history\n");
  }

  signal(SIGINT, catch_ctrl_c_and_exit);

  while (1)
  {
    sin_size = sizeof(struct sockaddr_in);
    if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
    {
      perror("\nError: ");
      return 0;
    }

    printf("New connection from [%s:%d]\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port)); /* prints client's IP */
                                                                                                 // echo(conn_sock);

    add_client(conn_sock);
    pthread_create(&tid, NULL, echo, &conn_sock);
  }
  close(listen_sock);
  return 0;
}