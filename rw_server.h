/***TCP服务器***/
#ifndef RW_SERVER_H
#define RW_SERVER_H

#include <netinet/in.h>
#include <stdbool.h>

#define SERVERPORT 4000		//服务器监听的端口
#define BACKLOG 10			//能接受的没有accept的连接
#define NUMBER 3			//能接受的没有accept的连接
#define COMMSIZE 5
#define DATASIZE 10
#define MAXSIZE 30			//10+5+5+5+5,data+comm+fd+fd+分隔符

//定义一个传递信息的格式
typedef struct messages
{
	int source_fd;
	int dest_fd;
	char comm[COMMSIZE];
	char data[DATASIZE];
}Msg;

//定义一个客户端信息的结构体
typedef struct client_info
{
	struct list_node list;
	struct list_node *info_head;
	struct sockaddr_in client_addr;
	pthread_t recv_tid;
	int client_fd;
	bool full;
}Info;


//建立一个线程和客户端进行通信
void* server_send(void *arg);
void* server_recv(void *arg);
void* search_send(int sock_fd, struct list_node *head, char *information);

#endif
