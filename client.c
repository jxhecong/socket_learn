/***TCP客户端***/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>

#define PORT 4000		//服务器监听的端口
#define MAXDATASIZE 10	//一次接收的最大字节数

int client_fd;

void* client_send(void* arg);
void* client_recv(void* arg);

int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr;
	struct hostent *server;	//服务器的地址信息
	pthread_t send_tid;
	pthread_t recv_tid;
	
	//接收要连接的服务器名或地址
	if ( argc != 2 )
	{
		fprintf(stderr,"usage:client hostname\n");
		exit(1);
	}
	//从服务器名获得一个hostent结构体的地址信息
	if ( (server=gethostbyname(argv[1])) == NULL )
	{
		herror("gethostbyname");
		exit(1);
	}
	//创建套接字用于连接
	if ( (client_fd = socket(AF_INET,SOCK_STREAM,0)) == -1 )
	{
		perror("socket");
		exit(1);
	}
	//服务器的协议、端口、地址信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr = *((struct in_addr *)server->h_addr);	//类型转换
	bzero(&(server_addr.sin_zero),8);
	//申请客户端套接字和服务器进行连接
	if ( connect(client_fd,(struct sockaddr *)&server_addr,\
		sizeof(struct sockaddr)) == -1 )
	{
		perror("connect");
		exit(1);
	}
	printf("client:success to connect server,client_fd is %d!\n",client_fd);
	//建立一个发送信息的线程
	if ( pthread_create(&send_tid,NULL,client_send,NULL) )
	{
		perror("send_pthread_create");
	}
	//接收服务器发来的字节信息
	if ( pthread_create(&send_tid,NULL,client_recv,NULL) )
	{
		perror("recv_pthread_create");
	}
	if ( pthread_join(send_tid,0) )
	{
		perror("send_pthread_join");
	}
	if ( pthread_join(recv_tid,0) )
	{
		perror("recv_pthread_join");
	}
	close(client_fd);
	return 0;
}

void* client_send(void* arg)
{
	char msg[MAXDATASIZE] = {0};
	int i = 0;
	while (1)
	{
		i = 0;
		memset(msg, 0, MAXDATASIZE);
		printf("send message to server or enter \"done\" to over!\n");
		setbuf(stdin,NULL);
		while ((msg[i] = getchar()) != '\n' && msg[i] != EOF && i < MAXDATASIZE)
		{
			i++;
		}
		/*if (MAXDATASIZE == i)
		{
			char c;
			while ((c = getchar()) != '\n' && c != EOF);
		}*/
		if (strncmp(msg, "done", 4) == 0)
		{
			break;
		}
		if ( send(client_fd,msg,strlen(msg),0) == -1 )
		{
			perror("send");
		}
	}
	printf("break out from send done!\n");
	pthread_exit(0);
}
void* client_recv(void* arg)
{
	int numbytes;
	char buf[MAXDATASIZE+1];
	while (1)
	{
		if ( (numbytes = recv(client_fd,buf,MAXDATASIZE,0)) < 1 )
		{
			perror("recv");
			exit(1);
		}
		//打印信息，关闭套接字
		buf[numbytes] = '\0';
		printf("received from server:%s",buf);
		if (strncmp(buf, "done", 4) == 0)
		{
			break;
		}
	}
	printf("break out from recv done!\n");
	pthread_exit(0);
}
