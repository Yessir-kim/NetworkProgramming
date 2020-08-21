#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

 /* 
	Success : 0 || Fail : -1
	<다른 서버로의 연결요청을 하는 함수>
	int connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen) ;
 */

void error_handling(char *message) ;

int
main(int argc, char* argv[])
{
		int sock ;
		struct sockaddr_in serv_addr ;
		char message[30] ;
		int str_len ;

		if(argc != 3)
		{
				printf("Usage : %s <IP> <PORT>\n", argv[0]) ;
				exit(1) ;
		}

		sock = socket(PF_INET, SOCK_STREAM, 0) ;
		if(sock == -1) error_handling("socket() error!\n") ;

		memset(&serv_addr, 0, sizeof(serv_addr)) ;
		serv_addr.sin_family = AF_INET ;
		serv_addr.sin_addr.s_addr = inet_addr(argv[1]) ;
		serv_addr.sin_port = htons(atoi(argv[2])) ;
		
		// sock이 클라이언트 소켓이 되는 시점 
		if(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) error_handling("connect() error!\n") ;

		str_len = read(sock, message, sizeof(message) - 1) ;
		if(str_len == -1) error_handling("read() error!\n") ;

		printf("Message from server : %s \n", message) ;
		close(sock) ;
		return 0 ;
}

void error_handling(char *message)
{
		fputs(message, stderr) ;
		exit(1) ;
}
