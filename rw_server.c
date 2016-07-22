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

//建立一个线程选择性向客户端发送信息
void* server_send(void *arg)
{
	char getstr[MAXSIZE] = {0};
	struct list_node *pos;
	struct list_node *list_head = (struct list_node *)arg;
	Msg msg_in;
	Info *node;
	while (1)
	{
		//pthread_mutex_lock(&mymutex);
		fgets(getstr, MAXSIZE, stdin);
		sscanf(getstr,"#%d:%s:%s#", &msg_in.sock_fd, msg_in.comm, msg_in.data);
		//pthread_mutex_unlock(&mymutex);
		list_for_each(pos, list_head)
		{
			node = (Info *)pos;
			if (0 == msg_in.sock_fd || node->client_fd == msg_in.sock_fd)
			{
				if ((send(node->client_fd, getstr, strlen(getstr), 0) == -1))
				{
					perror("send");
				}
				printf("send messge to client_fd %d!\n", node->client_fd);
			}
		}
		printf("search and send over!\n");
	}
	printf("break out from server_send!\n");
	pthread_exit(0);
}
//建立多个线程接收多个客户端的信息
void* server_recv(void *arg)
{
	Info *node = (Info *)arg;	//指向当前连接的客户端地址信息
	int numbytes = 0;
	char buf[MAXSIZE+1];
	while (1)
	{
		//接收客户端发来的字节信息
		if ((numbytes = recv(node->client_fd, buf, MAXSIZE, 0)) < 1)
		{
			perror("recv");
			exit(1);
		}
		//打印信息，关闭套接字
		buf[numbytes] = '\0';
		printf("received from fd%d:%s",node->client_fd,buf);
		/*if (strncmp(buf, "done", 4) == 0)
		{
			break;
		}*/
	}
	free(node);
	printf("break out from server_recv!\n");
	close(node->client_fd);
	pthread_exit(0);
}
