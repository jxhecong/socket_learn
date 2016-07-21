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
#include "list_hc.h"
#include "rw_server.h"

int server_fd;				//服务器的监听套接字
static int num = 0;			//客户端数

void main()
{
	//struct sockaddr_in类似struct sockaddr类型，相同大小
	struct sockaddr_in server_addr;		//服务器的地址，协议、IP、端口
	struct list_node *list_head;		//定义一个头节点
	Info *info_node;
	int sin_size = sizeof(struct sockaddr);	//存放sizeof(struck sockaddr_in)
	
	list_head = (struct list_node *)malloc(sizeof(struct list_node));
	list_init(list_head);	//初始化头节点
	printf("head init ok!\n");
	//创建监听套接字，进行错误检查
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
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
	while (1)
	{
		if (num < NUMBER)
		{
			//添加一个客户端节点
			info_node = (Info *)malloc(sizeof(Info));
			//阻塞主线程，等待新连接，接收客户端的地址信息
			if ((info_node->client_fd = accept(server_fd,\
				(struct sockaddr *)&info_node->client_addr, &sin_size)) == -1)
			{
				perror("accept");
				exit(1);
			}
			list_add_before(&info_node->list, list_head);
			num++;		//连接新客户端后，数目+1
			//打印客户端地址，网络字节顺序转成ASCALL
			printf("server:got connection from %s\n",\
				inet_ntoa(info_node->client_addr.sin_addr));
			//建立一个发送信息的线程
			if (pthread_create(&info_node->send_tid, NULL, server_send, (void *)info_node))
			{
				perror("pthread_create");
			}
			//建立一个接收信息的线程
			if (pthread_create(&info_node->recv_tid, NULL, server_recv, (void *)info_node))
			{
				perror("pthread_create");
			}
		}
	}
	/*
	//等待线程接收
	if (pthread_join(server_tid[2*num], 0))
	{
		perror("pthread_join");
	}
	//等待线程接收
	if (pthread_join(server_tid[2*num+1], 0))
	{
		perror("pthread_join");
	}
	*/
	close(server_fd);
}