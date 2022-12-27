 #include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/types.h>


int main()
{

    struct sockaddr_in myaddr ,serveraddr;
    int sockid;
    sockid=socket(AF_INET,SOCK_STREAM,0);
    memset(&myaddr,'0',sizeof(myaddr));
    myaddr.sin_family=AF_INET;
    myaddr.sin_port=htons(8888);
    myaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    if(sockid==-1)
    {
        perror("socket");
    }
    int len=sizeof(myaddr);
    if(connect(sockid,(const struct sockaddr*)&myaddr,len)==-1)
    {
        perror("connect");
    }
    fprintf(stdout,"Client Online....");
    char s[10000];


        //gets(s);
        //send(sockid,s,10000,0);
        recv(sockid,&s,10000,0);
        fprintf(stdout,"Server says....");
        puts(s);
        recv(sockid,&s,10000,0);
        fprintf(stdout,"Server says....");
        puts(s);

sleep(10);  
    close(sockid);
    return 0;
}