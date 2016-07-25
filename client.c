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
#define COMMSIZE 5
#define DATASIZE 10
#define MAXSIZE 30          //10+5+5+4,data+comm+fd+分隔符

int client_fd;
typedef struct messages
{
    int source_fd;
	int dest_fd;
    char comm[COMMSIZE];
	char data[MAXSIZE];
}Msg;
		  
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
	printf("client:success to connect server!\n");
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
	Msg msg;
	char getstr[MAXSIZE] = {0};
	while (1)
	{
		memset(getstr, 0, sizeof(getstr));
		memset(&msg, 0, sizeof(msg));
		printf("send message in format:#source_fd.comm.dest_fd.data#\n\
		\"#0.dele.0.data#\" to over this client,\n\
		\"#my_fd.exch.dest_fd.data#\" to send message to other client,\n\
		\"#0.serv.0.data#\" to send message to server,\n\
		\"#0.show.0.data#\" to get clients infomation!\n");
		fgets(getstr, MAXSIZE, stdin);
		fflush(stdin);
		if (sscanf(getstr,"#%d.%[^.].%d.%[^#]#", &msg.source_fd, msg.comm, &msg.dest_fd, msg.data) == -1)
		{   
			perror("sscanf");
		}
		if (strncmp(msg.comm, "serv", 4) == 0\
			|| strncmp(msg.comm, "dele", 4) == 0\
			|| strncmp(msg.comm, "show", 4) == 0\
			|| strncmp(msg.comm, "exch", 4) == 0)
		{
			if ( send(client_fd, getstr, strlen(getstr),0) == -1 )
			{
				perror("send");
			}
		}
		if (strncmp(msg.comm, "dele", 4) == 0)
		{
			break;
		}
	}
	//close(client_fd);
	printf("break out from send done!\n");
	pthread_exit(0);
}
void* client_recv(void* arg)
{
	int numbytes;
	char buf[MAXSIZE];
	Msg msg;
	while (1)
	{
		memset(&msg, 0, sizeof(msg));
		memset(buf, 0, sizeof(buf));
		if ( (numbytes = recv(client_fd, buf, MAXSIZE-1, 0)) < 1 )
		{
			perror("recv");
			exit(1);
		}
		//打印信息，关闭套接字
		buf[numbytes-1] = '#';
		buf[numbytes] = '\0';
		printf("received from server:%s\n", buf);
		fflush(stdout);
		if (sscanf(buf,"#%d.%[^.].%d.%[^#]#", &msg.source_fd, msg.comm, &msg.dest_fd, msg.data) == -1)
		{   
			perror("sscanf");
		}
		if (strncmp(msg.comm, "dele", 4) == 0)
		{
			break;
		}
	}
	printf("break out from recv done!\n");
	pthread_exit(0);
}
