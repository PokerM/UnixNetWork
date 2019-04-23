#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include<arpa/inet.h>
#include <unistd.h>

#define MAXLINE 1000
int main(int argc, char **argv){
	int sockfd, n;
	char recvline[MAXLINE];
	struct sockaddr_in servaddr;
    	if(argc != 2){
		printf("usage : %s ip\n", argv[0]);
		exit(1);
	}
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("error sockfd fail!\n");
		exit(1);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	// 连接的端口，使用TCP
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(2000);
	// 将第一个参数IP转化为网络字节序
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
		printf("error to tranfer IP\n");
		exit(1);
	} 
	// 连接服务器
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
		printf("error to connect server\n");
		exit(1);
	}
	// 读取服务器发送的时间，并在终端展示
	while(1){
		printf("test!\n");
		while((n = recv(sockfd, recvline, MAXLINE, 0)) > 0){
			recvline[n] = 0;
			if(fputs(recvline, stdout) == EOF){
				printf("fail to printf received data!\n");
				exit(1);
			}
		}
		if(n < 0){
			printf("read error\n");
			exit(1);
		}
	}
	exit(0);
}
