/***TCP服务器***/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "list_hc.h"
#include "rw_server.h"
//pthread_mutex_t mymutex = PTHREAD_MUTEX_INITALLIZER;

#define DEBUG

//从头节点查找目的fd并发送信息
void* search_send(int dest_fd, struct list_node *head, char *information)
{
	struct list_node *pos;
	Info *node;
	int torf = 1;
	for (pos = head->next; pos != head; pos = pos->next)
	{
		node = (Info *)pos;
		if (100 == dest_fd || node->client_fd == dest_fd)
		{
			if ((send(node->client_fd, information, strlen(information), 0) == -1))
			{
				perror("send");
			}
			torf = 0;
		}
	}
	if (torf)
	{
		printf("this client is not exist!\n");
	}
}

//建立一个线程选择性向客户端发送信息
void* server_send(void *node_arg)
{
	struct list_node *pos;
	struct list_node *list_head = (struct list_node *)node_arg;
	int numbytes = 0;
	int source_fd = 0;
	char getstr[MAXSIZE];
	Msg msg_in;
	Info *node;
	while (1)
	{
		memset(getstr, 0, sizeof(getstr));
		memset(&msg_in, 0, sizeof(msg_in));
	    printf("接收和发送的信息格式:命令/客户端socket_fd/信息数据\n\
			\"0/client_fd/data\" 向客户端发送信息，群发fd为100!\n");
		//pthread_mutex_lock(&mymutex);
		fgets(getstr, MAXSIZE-1, stdin);
		fflush(stdin);
		//pthread_mutex_unlock(&mymutex);
		numbytes = strlen(getstr);		//strlen不计算最后一个结束符
		if (getstr[numbytes-1] != '\n')
		{

			while(getchar() != '\n');
			printf("too much input!\n");
			//getstr[numbytes-2] = '#';
			//getstr[numbytes-1] = '\n';
			continue;
		}
		//对信息格式化取出分析
		if (sscanf(getstr, "%[^/]/%d/%s", msg_in.comm, &msg_in.dest_fd, msg_in.data) == -1)
		{
			perror("sscanf");
		}
#ifdef DEBUG
		printf("msg_in.dest_fd %d\n", msg_in.dest_fd);
#endif
		memset(getstr, 0, sizeof(getstr));
		sprintf(getstr, "%s/%d/%s", msg_in.comm, source_fd, msg_in.data);
		//搜索客户端并发送信息
		search_send(msg_in.dest_fd, list_head, getstr);
	}
	printf("break out from server_send!\n");
	pthread_exit(0);
}
//建立多个线程接收多个客户端的信息
void* server_recv(void *node_arg)
{
	Info *node = (Info *)node_arg;	//指向当前连接的客户端地址信息
	//struct list_node *listnode = &node->list;
	Info *tempnode;
	struct list_node *pos;
	Msg msg_in;
	int numbytes = 0;
	char buf[MAXSIZE];
	char nodeinfo[MAXSIZE];
	char allnode[MAXSIZE];
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		memset(&msg_in, 0, sizeof(msg_in));
		//接收客户端发来的字节信息
		if ((numbytes = recv(node->client_fd, buf, MAXSIZE-1, 0)) == -1)
		{
			perror("recv");
		}
#ifdef DEBUG
		printf("recv buf %s\n", buf);
#endif
		//buf[numbytes-1] = '#';		//recv的返回个数包括结束符
		//buf[numbytes] = '\0';
		//对接收的信息格式化取出判断
		if (sscanf(buf, "%[^/]/%d/%s", msg_in.comm, &msg_in.dest_fd, msg_in.data) == -1)
		{
			perror("sscanf");
		}
#ifdef DEBUG
		printf("sscanf buf %s\n", buf);
#endif
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s/%d/%s", msg_in.comm, node->client_fd, msg_in.data);
#ifdef DEBUG
		printf("sprintf comm %s\n", msg_in.comm);
		printf("sprintf buf %s\n", buf);
#endif
		//单纯在服务器中打印信息
		if (0 == strncmp(msg_in.comm, "serv", 4))
		{
			printf("received from client %d:%s\n", node->client_fd, buf);
			fflush(stdout);
		}
		//转发客户端的信息至另一个客户端
		else if (0 == strncmp(msg_in.comm, "exch", 4))
		{
			printf("exchange messge to client %d\n", msg_in.dest_fd);
			search_send(msg_in.dest_fd, node->info_head, buf);
		}
		//给客户端发送在线客户端信息
		else if (0 == strncmp(msg_in.comm, "show", 4))
		{
			memset(nodeinfo, 0, sizeof(nodeinfo));
			memset(allnode, 0, sizeof(allnode));
			printf("send information to client %d\n", msg_in.source_fd);
			list_for_each(pos, node->info_head)
			{
				tempnode = (Info *)pos;
				sprintf(nodeinfo, "client fd-%d, addr-%s :", tempnode->client_fd,\
					inet_ntoa(tempnode->client_addr.sin_addr));
				strcat(allnode, nodeinfo);
			}
			if ((send(node->client_fd, allnode, strlen(allnode), 0) == -1))
			{
				perror("send");
			}
		}
		//删除该客户端的套接字，跳出接收循环关闭该线程
		else if (0 == strncmp(msg_in.comm, "dele", 4))
		{
			printf("delete client %d\n", node->client_fd);
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
