/***TCP服务器***/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
//#include <stdbool.h>
#include "list_hc.h"
#include "rw_server.h"

int server_fd;				//服务器的监听套接字

void main()
{
	//struct sockaddr_in类似struct sockaddr类型，相同大小
	struct sockaddr_in server_addr;		//服务器的地址，协议、IP、端口
	struct list_node *list_head;		//定义一个头节点
	struct list_node *list_pos;			//定义一个节点
	pthread_t send_tid;
	Info *info_node;
	int sin_size = sizeof(struct sockaddr);	//存放sizeof(struck sockaddr_in)
	char nodeinfo[MAXSIZE];
	int num = 0;			//客户端数

	list_head = (struct list_node *)malloc(sizeof(struct list_node));
	list_init(list_head);				//初始化头节点
	//创建监听套接字，进行错误检查
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	printf("server_fd: %d\n", server_fd);
	//对服务器地址信息进行赋值填充
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;			//IPv4类型协议，主机字节顺序
	server_addr.sin_port = htons(SERVERPORT);	//监听端口，转换成网络字节顺序
	server_addr.sin_addr.s_addr = INADDR_ANY;	//赋零值，本机IP，网络字节顺序
	//bzero(&(server_addr.sin_zero),8);			//其余空间清零
	//绑定监听套接字和端口等信息，进行错误检查
	if (bind(server_fd, (struct sockaddr *)&server_addr, sin_size) == -1)
	{
		perror("bind");
		exit(1);
	}
	//监听网络
	if (listen(server_fd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}
	printf("server is begin to accept!\n");
	//建立一个接收终端输入并发送信息的线程
	if (pthread_create(&send_tid, NULL, server_send, (void *)list_head))
	{
		perror("send pthread_create");
	}

	while (1)
	{
		num = 0;
		//添加一个客户端节点
		info_node = (Info *)malloc(sizeof(Info));
		//阻塞主线程，等待新连接，接收客户端的地址信息
		if ((info_node->client_fd = accept(server_fd,\
			(struct sockaddr *)&info_node->client_addr, &sin_size)) == -1)
		{	
			perror("accept");
		}
		sprintf(nodeinfo, "本机fd:%d, addr:%s", info_node->client_fd, inet_ntoa(info_node->client_addr.sin_addr));
		printf("connect a new client: %s\n", nodeinfo);
		send(info_node->client_fd, nodeinfo, strlen(nodeinfo), 0);
		list_for_each(list_pos, list_head)
		{
			num++;
		}
		if (num == NUMBER)		//连接客户端数目超额
		{
			sleep(1);
			if (send(info_node->client_fd, "dele/0/client full", 20, 0) == -1)
			{
				printf("send");
			}
			printf("the connected clients are full, cancle this connect!\n");
		}
		else
		{
			info_node->info_head = list_head;
			list_add_before(&info_node->list, list_head);	//将新客户端加入链表中，并打印信息
			printf("server:get new connection from %s, allocation fd is %d!\n",\
				inet_ntoa(info_node->client_addr.sin_addr), info_node->client_fd);
			//if (send(info_node->client_fd, ) == -1)
			//建立一个接收信息的线程
			if (pthread_create(&info_node->recv_tid, NULL, server_recv, (void *)info_node))
			{
				perror("recv pthread_create");
			}
		}
	}
	if (pthread_join(send_tid, 0))
	{
		perror("send_pthread_join");
	}
	close(server_fd);
}
