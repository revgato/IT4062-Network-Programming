#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "chatroom.h"

// taikhoan.txt groupchat.txt

#define PORT 5550  /* Port that will be opened */
#define BACKLOG 20 /* Number of allowed connections */
#define BUFF_SIZE 1024

/* Receive and echo message to client */
void split_message(char *message, char *username, char *password){
	int i = 0;
	while(message[i] != '-'){
		username[i] = message[i];
		i++;
	}
	username[i] = '\0';
	i++;
	int j = 0;
	while(message[i] != '\0'){
		password[j] = message[i];
		i++;
		j++;
	}
	password[j] = '\0';
}
void *echo(void *);

int main()
{
	int listenfd, *connfd;
	struct sockaddr_in server;	/* server's address information */
	struct sockaddr_in *client; /* client's address information */
	int sin_size;
	pthread_t tid;
	char bao[] = "Dang nhap thanh cong";

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

	if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	if (listen(listenfd, BACKLOG) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	sin_size = sizeof(struct sockaddr_in);
	client = malloc(sin_size);
	while (1)
	{
		connfd = malloc(sizeof(int));
		if ((*connfd = accept(listenfd, (struct sockaddr *)client, &sin_size)) == -1)
			perror("\nError: ");

		printf("You got a connection from %s\n", inet_ntoa((*client).sin_addr)); /* prints client's IP */

		/* For each client, spawns a thread, and the thread handles the new client */
		// pthread_create(&tid, NULL, &echo, connfd);
		pthread_create(&tid, NULL, &echo, connfd);
	}

	close(listenfd);
	return 0;
}

void *echo(void *arg)
{
	int connfd;
	int bytes_sent, bytes_received;
	char buff[BUFF_SIZE + 1];
	char username[50], password[50], message[100];
	user_list *ulist = NULL;
	read_user_data(&ulist);


	connfd = *((int *)arg);
	free(arg);
	pthread_detach(pthread_self());

	bytes_received = recv(connfd, buff, BUFF_SIZE, 0); // blocking
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.");
	
	split_message(buff, username, password);

	if(login(ulist, username, password) == 1){
		bytes_sent = send(connfd, "Dang nhap thanh cong", strlen("Dang nhap thanh cong"), 0);
	}
	else{
		bytes_sent = send(connfd, "Dang nhap that bai", strlen("Dang nhap that bai"), 0);
	}
	


	// bytes_sent = send(connfd, buff, bytes_received, 0); /* send to the client welcome message */
	if (bytes_sent < 0)
	{
		perror("\nError: ");

		close(connfd);
	}
}
