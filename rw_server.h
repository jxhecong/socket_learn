/***TCP服务器***/
#ifndef RW_SERVER_H
#define RW_SERVER_H

#include <netinet/in.h>

#define SERVERPORT 4000		//服务器监听的端口
#define BACKLOG 10			//能接受的没有accept的连接
#define NUMBER 10			//能接受的没有accept的连接
#define MAXDATASIZE 10

//定义一个连接信息的结构体
typedef struct client_info
{
	struct list_node list;
	struct sockaddr_in client_addr;
	pthread_t send_tid;
	pthread_t recv_tid;
	int client_fd;
}Info;

//建立一个线程和客户端进行通信
void* server_send(void *arg);
void* server_recv(void *arg);

#endif
