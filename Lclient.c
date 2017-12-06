/*
板子客户端程序
*/
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


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

	/*3、打开驱动文件*/
	int fdr = open("/dev/mydevice", O_RDWR);//fdr为驱动文件描述符
	if(fdr < 0)
	{
		perror("open");
		exit(-1);
	}

	/*核心操作*/
	while(1)
	{	
		int num = 0;        
		read(fd, &num, 4);//从服务端读取一个字节的内容到fd里
		if(num == 0)
		{
			break;
		}
		write(fdr, &num, 4);//把这个内容写到fdr中
		printf("\n");
	}
	close(fd);	
	close(fdr);
}
