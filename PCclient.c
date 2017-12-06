/*
PC客户端
*/
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, const char *argv[])
{
	if(argc < 3)
	{
		printf("请附上ip和端口,空格隔开\n");
		exit(-1);
	}

	/*1、创建套接字*/
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}	

	/*2、连接服务器*/
	struct sockaddr_in sockin;
	bzero(&sockin, sizeof(sockin));
	sockin.sin_family = AF_INET;
	sockin.sin_port = htons(atoi(argv[2]));//端口
	inet_pton(AF_INET, argv[1], &sockin.sin_addr);
	if(connect(fd, (struct sockaddr *)&sockin, sizeof(sockin)) < 0)
	{
		perror("connect");
		exit(EXIT_FAILURE);
	}
	
	/*核心操作*/
	while(1)
	{
		int num = 0;

	loop:printf("请输入，'1' 为开灯，'2' 为关灯, '0'为退出:\n");
		scanf("%d", &num);

		if(num != 1 || num != 2 || num != 0)
		{
			printf("请输入正确的数字！\n");
			goto loop;
		}

		write(fd, &num, 4); //写入一个用户输入的内容到服务端的fd里
		if(num == 0)
		{
			break;
		}
		printf("\n");
	}
	close(fd);//关闭	
}
