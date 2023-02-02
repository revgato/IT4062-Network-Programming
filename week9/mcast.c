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

#include "communicate.h"

struct sockaddr_in gSock; // socket for sending to the multicast group
neighbour_node *mtable = NULL;

int main(int argc, char *argv[]){
  struct ip_mreq mreq; // multicast group structure
  struct in_addr localInterface; // local interface structure
  struct sockaddr_in localSock; // local socket structure
  int sendfd, recvfd;
  char command[50];

  pthread_t recv_thread, send_thread, check_thread;

  // Create a socket for sending to the multicast group
  sendfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sendfd < 0){
    perror("opening datagram socket");
    return 0;
  }

  memset(&gSock, 0, sizeof(gSock));
  gSock.sin_family = AF_INET;
  gSock.sin_addr.s_addr = inet_addr(argv[1]);
  gSock.sin_port = htons(atoi(argv[2]));

  // Disable loopback so you do not receive your own datagrams
  char loopch = 0;

  if (setsockopt(sendfd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0){
    perror("setting IP_MULTICAST_LOOP:");
    close(sendfd);
    return 0;
  }

  // Set local interface for outbound multicast datagrams
  localInterface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sendfd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0){
    perror("setting local interface");
    close(sendfd);
    return 0;
  }

  // Create a socket for receiving datagrams
  recvfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (recvfd < 0){
    perror("opening datagram socket");
    return 0;
  }

  int reuse = 1;

  if (setsockopt(recvfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0){
    perror("setting SO_REUSEADDR");
    close(recvfd);
    return 0;
  }

  // Bind to the proper port number with the IP address
  // specified as INADDR_ANY
  memset(&localSock, 0, sizeof(localSock));
  localSock.sin_family = AF_INET;
  localSock.sin_port = htons(atoi(argv[2]));
  localSock.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(recvfd, (struct sockaddr *)&localSock, sizeof(localSock))){
    perror("binding datagram socket");
    close(recvfd);
    return 0;
  }

  // Join the multicast group
  mreq.imr_multiaddr.s_addr = inet_addr(argv[1]);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(recvfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
    perror("adding multicast group");
    close(recvfd);
    return 0;
  }

  // Create a thread for receiving datagrams
  if (pthread_create(&recv_thread, NULL, send_pthread, (void *)&recvfd) != 0){
    perror("creating recv_thread");
    close(recvfd);
    return 0;
  }

  // Create a thread for sending datagrams
  if (pthread_create(&send_thread, NULL, receive_pthread, (void *)&sendfd) != 0){
    perror("creating send_thread");
    close(sendfd);
    return 0;
  }

  // Create a thread for checking the neighbour table
  if (pthread_create(&check_thread, NULL, check_pthread, NULL) != 0){
    perror("creating check_thread");
    return 0;
  }

  // Infinite loop for user input
  while (1){
    printf("Command: ");
    scanf("%[^\n]%*c", command);

    // If user enter "quit", terminate the program
    if (strcmp(command, "quit") == 0){
      pthread_cancel(recv_thread);
      pthread_cancel(send_thread);
      pthread_cancel(check_thread);
      printf("Bye\n");
      break;
    } else if (strcmp(command, "print mtable") == 0){
      traverse();
    } else{
      printf("Wrong command\n");
    }
  }

  // Close the sockets and terminate the program
  free_list();
  close(sendfd);
  close(recvfd);
  return 0;


}