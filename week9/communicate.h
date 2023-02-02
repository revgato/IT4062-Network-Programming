#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MESSAGE "hello"
extern struct sockaddr_in gSock; // socket for sending to the multicast group

// Linked list for neighbours node
typedef struct neighbour_node_t
{
  char ip[16]; // ip address of node
  time_t last_activate;
  struct neighbour_node_t *next;
} neighbour_node;

extern neighbour_node *mtable; // head of linked list for neighbours

// Add new node to the list
void add_node(char *ip){
  neighbour_node *new_node = (neighbour_node *)malloc(sizeof(neighbour_node));
  strcpy(new_node->ip, ip);
  new_node->last_activate = time(0);
  new_node->next = mtable;
  mtable = new_node;
}

// Delete node from the list
void delete_node(char *ip){
  neighbour_node *current = mtable;
  neighbour_node *previous = NULL;
  while(current != NULL){
    if(strcmp(current->ip, ip) == 0){
      if(previous == NULL){
        mtable = current->next;
      }else{
        previous->next = current->next;
      }
      free(current);
      break;
    }
    previous = current;
    current = current->next;
  }
}

// Print all nodes in the list
void traverse(){
  printf("List of neighbours:\n");
  neighbour_node *current = mtable;
  while(current != NULL){
    printf("%s\n", current->ip);
    current = current->next;
  }
}

// Find node in the list by ip address
neighbour_node *find_node(char *ip){
  neighbour_node *current = mtable;
  while(current != NULL){
    if(strcmp(current->ip, ip) == 0){
      return current;
    }
    current = current->next;
  }
  return NULL;
}

// Update last activate time of node
void update_node(char *ip){
  neighbour_node *current = mtable;
  while(current != NULL){
    if(strcmp(current->ip, ip) == 0){
      current->last_activate = time(0);
      break;
    }
    current = current->next;
  }
}

// Free all nodes in the list
void free_list(){
  neighbour_node *current = mtable;
  neighbour_node *next = NULL;
  while(current != NULL){
    next = current->next;
    free(current);
    current = next;
  }
}

// pthread function for receiving multicast message
void *receive_pthread(void *arg){
  pthread_detach(pthread_self());

  int sock = *(int *)arg;
  char buf[100];
  int bytes_received;
  struct sockaddr_in from;
  socklen_t fromlen = sizeof(from);
  char ip[16];


  while(1){
    bytes_received = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen);
    if(bytes_received < 0){
      perror("recvfrom");
      exit(1);
    }
    buf[bytes_received] = '\0';
    
    if (strcmp(buf, MESSAGE) == 0) {
      // Add new node to the list
      strcpy(ip, inet_ntoa(from.sin_addr));
      neighbour_node *node = find_node(ip);
      if(node == NULL){
        add_node(ip);
      }else{
        update_node(ip);
      }
    }
  }
}

// pthread function for sending multicast message
void *send_pthread(void *arg){
  pthread_detach(pthread_self());

  int sock = *(int *)arg;
  int bytes_sent;
  while(1){
    bytes_sent = sendto(sock, MESSAGE, strlen(MESSAGE), 0, (struct sockaddr *)&gSock, sizeof(gSock));
    if(bytes_sent < 0){
      perror("sendto");
      exit(1);
    }
    // Send multicast message every 10 seconds
    sleep(10);
  }
}

// pthread function for checking neighbour nodes (delete nodes that haven't been activated for 30 seconds)
void *check_pthread(void *arg){
  pthread_detach(pthread_self());

  char ip[16];
  while(1){
    neighbour_node *current = mtable;
    while(current != NULL){
      if(time(0) - current->last_activate > 30){
        strcpy(ip, current->ip);
        delete_node(ip);
      }
      current = current->next;
    }
    // Check every 30 seconds
    sleep(30);
  }
}