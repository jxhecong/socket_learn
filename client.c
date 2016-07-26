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
//#include <signal.h>

#define PORT 4000		//服务器监听的端口
#define COMMSIZE 4 
#define DATASIZE 1000
#define MAXSIZE 1024          //10+5+5+4,data+comm+fd+分隔符
//#define DEBUG

struct sockaddr_in server_addr;
int client_fd;
pthread_t send_tid;
pthread_t recv_tid;

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
	struct hostent *server;	//服务器的地址信息
	int flag = 1;
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
	//printf("socket fd %d\n", client_fd);
	//服务器的协议、端口、地址信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr = *((struct in_addr *)server->h_addr);	//类型转换
	bzero(&(server_addr.sin_zero),8);
	//申请客户端套接字和服务器进行连接
	if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
	{
		perror("setsockopt");
	}
	if ( connect(client_fd,(struct sockaddr *)&server_addr,\
		sizeof(struct sockaddr)) == -1 )
	{
		perror("connect");
		exit(1);
	}
	/*if ( (numbytes = recv(client_fd, buf, MAXSIZE-1, 0)) < 1 )
	{
		perror("recv");
	}
	printf("received from server:%s\n", buf);*/
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
	int numbytes;
	char getstr[MAXSIZE] = {0};
	char cpystr[MAXSIZE] = {0};
	while (1)
	{
		memset(getstr, 0, sizeof(getstr));
		memset(&msg, 0, sizeof(msg));
		printf("接收和发送信息格式: 命令/对端socket_fd/信息数据\n\
		\"dele/0/data\" 结束本客户端的通信,\n\
		\"exch/dest_fd/data\" 向另一个客户端发消息,\n\
		\"serv/0/data\" 向服务器发送信息,\n\
		\"show/0/data\" 查询在线客户端!\n");
		fgets(getstr, MAXSIZE-1, stdin);
		fflush(stdin);
		numbytes = strlen(getstr);
		if (getstr[numbytes-1] != '\n')
		{
			while(getchar() != '\n');
			printf("too much input!\n");
			continue;
		}
#ifdef DEBUG
		printf("fgets getstr:%s\n", getstr);
#endif
		//对输入进行格式化提取、判断
		//memcpy(cpystr, getstr, strlen(getstr));
		if (sscanf(getstr,"%[^/]/%d/%s", msg.comm, &msg.dest_fd, msg.data) == -1)
		{   
			perror("sscanf");
		}
#ifdef DEBUG
		printf("sscanf getstr:%s\n", getstr);
#endif
		//memset(getstr, 0, sizeof(getstr));
		//sprintf(getstr, "%s.%d.%s", msg.comm, msg.dest_fd, msg.data);
#ifdef DEBUG
		printf("sprintf getstr:%s\n", getstr);
#endif
		//命令正确则发送给服务器
		if (strncmp(msg.comm, "serv", 4) == 0 || strncmp(msg.comm, "dele", 4) == 0\
			|| strncmp(msg.comm, "show", 4) == 0 || strncmp(msg.comm, "exch", 4) == 0)
		{
			if ( send(client_fd, getstr, strlen(getstr),0) == -1 )
			{
				perror("send");
			}
		}
		else
		{
			printf("wrong commend input!\n");
		}
		//如果输入删除命令，跳出循环退出发送线程
		if (strncmp(msg.comm, "dele", 4) == 0)
		{
			break;
		}
	}
	printf("break out from send done!\n");
	pthread_exit(0);
}

void* client_recv(void* arg)
{
	int numbytes;
	char buf[MAXSIZE];
	//char cpybuf[MAXSIZE];
	Msg msg;
	while (1)
	{
		memset(&msg, 0, sizeof(msg));
		memset(buf, 0, sizeof(buf));
		if ( (numbytes = recv(client_fd, buf, MAXSIZE-1, 0)) == -1 )
		{
			perror("recv");
		}
		if (strlen(buf) == 0)
		{
			//continue;
			printf("can not recv data from server, try to connect server again!\n");
			if (connect(client_fd, (struct sockaddr *)&server_addr,\
				sizeof(struct sockaddr)) == -1)
			{
				perror("connect");
				//exit(1);
			}
			sleep(5);
			continue;
		}
		printf("received from server: %s\n", buf);
		fflush(stdout);
		//对接收进行格式化提取判断
		//memcpy(cpybuf, buf, strlen(buf));
		if (sscanf(buf,"%[^/]/%d/%s", msg.comm, &msg.source_fd, msg.data) == -1)
		{   
			perror("sscanf");
		}
		//获得服务器返回的删除命令，退出接收线程
		if (strncmp(msg.comm, "dele", 4) == 0)
		{
			break;
		}
	}
	//kill(send_tid, 0);
	printf("break out from recv done!\n");
	pthread_exit(0);
}
