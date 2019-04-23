#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 1024
#define LISTENQ 16
int main(){
	int listenfd, connfd;
	struct sockaddr_in servaddr, clientaddr;
	char buff[MAXLINE];
	time_t ticks;
	// 使用TCP
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	// 设置服务器的IP地址为任何地址和端口号13，多余的空间用用零填充。这里的地址都是网络字节序
	bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(2000);
	// 将socket绑定为固定的地址，转化成sockaddr
	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
		printf("error to bind the address\n");
		exit(1);
	}
	// 开始监听，监听队列最大为16
	listen(listenfd, LISTENQ);
	char addr[MAXLINE];
	socklen_t len;
	while(1){
		len = sizeof(clientaddr);
        printf("server opened!\n");
		// 三次握手过后，可以获得客户端句柄，同时获得客户端地址，后面两个参数都要设置
		connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
		inet_ntop(((struct sockaddr*)&clientaddr)->sa_family, (void*)&clientaddr.sin_addr, addr, MAXLINE);
		printf("client %s:%d connect server...\n", addr, ntohs(clientaddr.sin_port));
		// 发送服务端时间给客户端
/* 		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks)); */
		sprintf(buff,"hello");
		send(connfd, buff, strlen(buff), 0);
		// 由服务端关闭，发生四次挥手，发送FIN
		close(connfd);
	}
	return 0;
}
