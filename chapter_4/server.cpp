#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

using namespace std;

#define MAXLINE 1024
#define LISTENQ 16
void doit(int connfd){
	char recvline[MAXLINE];
	int n;
	cout<<"waiting..."<<endl;
	while((n = recv(connfd, recvline, MAXLINE, 0)) > 0){
		recvline[n] = '\0';
		cout<<"test:"<<recvline<<endl;
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

in_port_t sockfd_to_port(int sockfd){
	union{
		struct sockaddr_in sa;
		char data[MAXLINE];
	} un;
	socklen_t len;
	len = MAXLINE;
	if(getsockname(sockfd, ( sockaddr*)un.data,&len)<0){
		return -1;
	}
	return (un.sa.sin_port);
}
int main(){
	int listenfd, connfd;
	struct sockaddr_in servaddr, clientaddr;
	char buff[MAXLINE];
	time_t ticks;
	pid_t pid;
	// 使用TCP
	/* 
	param1:AF_INET ipv4
	param2:SOCK_STREAM data stream socket
	param3:TCP UDP SCTP or 0(default)
	return socket descriptor
	 */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));//将目标字节串指定数目的的字节置0
    servaddr.sin_family = AF_INET;//IPV$
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(0);//内核分配端口
	// 将socket绑定为固定的地址，转化成sockaddr
	/* 
	将本地协议地址（ip+port）赋予一个套接字
	 */
	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
		printf("error to bind the address\n");
		exit(1);
	}
	in_port_t myport = sockfd_to_port(listenfd);
	int port = ntohs(myport);
	cout<<"port:"<<port<<endl;
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
/* 		sprintf(buff,"hello\n");
		send(connfd, buff, strlen(buff), 0); */
		if((pid = fork()) == 0){
			close(listenfd);
			doit(connfd);
			close(connfd);
			exit(0);
		}
		// 发送服务端时间给客户端
/* 		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks)); */
		// 由服务端关闭，发生四次挥手，发送FIN
		//else 
		else
			close(connfd);
	}
	return 0;
}
