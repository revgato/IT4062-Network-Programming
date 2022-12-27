#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{

	struct sockaddr_in myaddr, clientaddr;
	int sockid, newsockid;
	sockid = socket(AF_INET, SOCK_STREAM, 0);
	memset(&myaddr, '0', sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(8888);
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (sockid == -1)
	{
		perror("socket");
	}
	int len = sizeof(myaddr);
	if (bind(sockid, (struct sockaddr *)&myaddr, len) == -1)
	{
		perror("bind");
	}
	if (listen(sockid, 10) == -1)
	{
		perror("listen");
	}
	int pid, new;
	static int counter = 0;
	for (;;)
	{
		new = accept(sockid, (struct sockaddr *)&clientaddr, &len);

		if ((pid = fork()) == -1)
		{
			close(new);
			continue;
		}
		else if (pid > 0)
		{
			close(new);
			counter++;
			printf("here2\n");
			continue;
		}
		else if (pid == 0)
		{
			char buf[100];

			counter++;
			printf("here 1\n");
			snprintf(buf, sizeof buf, "hi %d", counter);
			send(new, buf, strlen(buf), 0);
			close(new);
			break;
		}
	}
	printf("here3");
	close(sockid);
	return 0;
}