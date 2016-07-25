/***TCP服务器***/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <sys/wait.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <pthread.h>
#include "list_hc.h"
#include "rw_server.h"
//pthread_mutex_t mymutex = PTHREAD_MUTEX_INITALLIZER;

//从头节点查找目的fd并发送信息
void* search_send(int sock_fd, struct list_node *head, char *information)
{
	struct list_node *pos;
	Info *node;
	for (pos = head->next; pos != head; pos = pos->next)
	{
		node = (Info *)pos;
		if (100 == sock_fd || node->client_fd == sock_fd)
		{
			if ((send(node->client_fd, information, strlen(information), 0) == -1))
			{
				perror("send");
			}
			printf("send getstr %s!\n", information);
		}
	}
	printf("search or send over!\n");
}

//建立一个线程选择性向客户端发送信息
void* server_send(void *arg)
{
	char getstr[MAXSIZE];
	int numbytes = 0;
	struct list_node *pos;
	struct list_node *list_head = (struct list_node *)arg;
	Msg msg_in;
	Info *node;
	while (1)
	{
		memset(getstr, 0, sizeof(getstr));
		memset(&msg_in, 0, sizeof(msg_in));
	    printf("send message in format:#source_fd.comm.dest_fd.data#\n\
			\"#0.0.client_fd.data#\" to get clients infomation!\n");
		//pthread_mutex_lock(&mymutex);
		fgets(getstr, MAXSIZE, stdin);
		fflush(stdin);
		numbytes = strlen(getstr);
		getstr[numbytes-2] = '#';
		getstr[numbytes-1] = '\0';
		//pthread_mutex_unlock(&mymutex);
		if (sscanf(getstr, "%*[^.].%*[^.].%d.%*[^#]", &msg_in.dest_fd) == -1)
		{
			perror("sscanf");
		}
		search_send(msg_in.dest_fd, list_head, getstr);	//搜索目的fd并发送信息
	}
	printf("break out from server_send!\n");
	pthread_exit(0);
}
//建立多个线程接收多个客户端的信息
void* server_recv(void *arg)
{
	Info *node = (Info *)arg;	//指向当前连接的客户端地址信息
	struct list_node *listnode = &node->list;
	struct list_node *pos = &node->list;
	Msg msg_in;
	int numbytes = 0;
	char buf[MAXSIZE];
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		memset(&msg_in, 0, sizeof(msg_in));
		//接收客户端发来的字节信息
		if ((numbytes = recv(node->client_fd, buf, MAXSIZE-1, 0)) < 1)
		{
			perror("recv");
			exit(1);
		}
		buf[numbytes-1] = '#';
		buf[numbytes] = '\0';
		if (sscanf(buf, "#%d.%[^.].%d.%[^#]#", &msg_in.source_fd, msg_in.comm, &msg_in.dest_fd, msg_in.data) == -1)
		{
			perror("sscanf");
		}
		if (0 == strncmp(msg_in.comm, "serv", 4))
		{
			printf("buf = %s\n", buf);
			printf("received from fd %d:%s\n", node->client_fd, buf);	//打印信息
			fflush(stdout);
		}
		else if (0 == strncmp(msg_in.comm, "exch", 4))
		{
			printf("exchange messge to client_fd %d!\n", msg_in.dest_fd);
			search_send(msg_in.dest_fd, node->info_head, buf);	//搜索并转发
		}
		else if (0 == strncmp(msg_in.comm, "show", 4))
		{
			printf("send clients information to client %d!\n", msg_in.source_fd);
			/*list_for_each(pos, listnode)
			{
				if ((send(node->client_fd, "show infomation", 15, 0) == -1))
				{
					perror("send");
				}
			}*/
		}
		else if (0 == strncmp(msg_in.comm, "dele", 4))
		{
			printf("delete client %d!\n", msg_in.source_fd);
			if ((send(node->client_fd, buf, strlen(buf), 0) == -1))
			{
				perror("send");
			}
			break;
		}
		else
		{
			printf("wrong commend input!\n");
		}
	}
	list_del_node(&node->list);
	free(node);
	printf("break out from server_recv!\n");
	close(node->client_fd);
	pthread_exit(0);
}
