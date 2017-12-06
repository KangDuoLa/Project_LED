/*
服务器程序（可云）
*/
#include <unistd.h>
#include <sys/socket.h>  //套接字相关
#include <arpa/inet.h> //因特网地址结构体 struct sockaddr_in定义
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(void)
{
	/*1、创建套接字*/
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}	

    /*2、将套接字和地址进行绑定*/
	/*2-1 先准备地址结构体*/
	struct sockaddr_in sockin;
	bzero(&sockin, sizeof(sockin));
	sockin.sin_family = AF_INET;//地址族
	sockin.sin_port = htons(2020);//本机监听端口
	sockin.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY的值0

	/*2-2 绑定 */
	if(bind(listen_fd, (struct sockaddr *)&sockin, sizeof(sockin)) < 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	/*3、开放套接字,开始监听*/
	if(listen(listen_fd, 10) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/*4、从队列中取一个连接, 进行通信*/
	while(1) //大循环，只给板子连接
	{
		struct sockaddr_in clientaddr;
		bzero(&clientaddr, sizeof(clientaddr));
		size_t len = sizeof(clientaddr);//长度必须要有初始值
	
		int fd1 = accept(listen_fd, (struct sockaddr *)&clientaddr, &len);	//fd1为板子客户端
		printf("板子客户端已连接\n");

		if(fd1 < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		while(1)//中循环，给遥控端连接
		{
			int fd2 = accept(listen_fd, (struct sockaddr *)&clientaddr, &len);	//fd2为遥控客户端
			printf("遥控客户端已连接\n");

			if(fd2 < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			while(1)//小循环（核心）
			{
				int num = 0;
				read(fd2, &num, 4);//读取PC端发送过来的内容
				if(num == 0)
				{
					break;
				}   
				write(fd1, &num, 4);//将PC端的内容发给板子端
			}
			close(fd2);//遥控端退出连接，进入大循环
		}
		close(fd1);//板子客户端
	}
	close(listen_fd);
}
