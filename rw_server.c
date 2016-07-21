/***TCP服务器***/
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <sys/wait.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <pthread.h>
#include "list_hc.h"
#include "rw_server.h"

//建立一个线程向客户端发送信息
void* server_send(void *arg)
{
	Info *node = (Info *)arg;
	char msg[MAXDATASIZE] = {0};
	int i = 0;
	while (1)
	{
		i = 0;
		memset(msg, 0, MAXDATASIZE);
		printf("send message to %d or enter \"done\" to over!\n",node->client_fd);
		setbuf(stdin, NULL);
		while ((msg[i] = getchar()) != '\n' && msg[i] != EOF\
			&& i < MAXDATASIZE)
		{
			i++;
		}
		if (strncmp(msg, "done", 4) == 0)
		{
			break;
		}
		if (send(node->client_fd, msg, strlen(msg), 0) == -1)
		{
			perror("send");
		}
	}
	free(node);
	printf("break out from send done!\n");
	pthread_exit(0);
}
//建立一个线程接收客户端信息
void* server_recv(void *arg)
{
	Info *node = (Info *)arg;
	int numbytes = 0;
	char buf[MAXDATASIZE+1];
	while (1)
	{
		//接收客户端发来的字节信息
		printf("client_fd is:%d\n",node->client_fd);
		if ((numbytes = recv(node->client_fd, buf, MAXDATASIZE, 0)) < 1)
		{
			perror("recv");
			exit(1);
		}
		//打印信息，关闭套接字
		buf[numbytes] = '\0';
		printf("received from fd%d:%s",node->client_fd,buf);
		if (strncmp(buf, "done", 4) == 0)
		{
			break;
		}
	}
	free(node);
	printf("break out from recv done!\n");
	close(node->client_fd);
	pthread_exit(0);
}
