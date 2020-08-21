#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* Success : file descriptor || Fail : -1 
	<소켓을 쓰기위해서 기본적으로 생성하는 함수>
	int socket(int domain, int type, int protocol) ;

 * Success : 0 || Fail : -1
	<소켓을 쓸 수 있도록 할당하는 함수>
 	int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen) ;

 * Success : 0 || Fail : -1
 	<소켓을 연결요청이 가능한 상태로 설정해주는 함수>
	int listen(int sockfd, int backlog) ;

 * Success : file descriptor || Fail : -1
	<데이터 송수신을 위한 연결요청이 들어오면 이를 허락해주는 함수>
	int accept(int sockfd, struct sockaddr * addr, socklen_t *addrlen) ;
*/

void error_handling(char *message) ;

int 
main(int argc, char *argv[])
{
	int serv_sock ;
	int clnt_sock ;

	struct sockaddr_in serv_addr ;
	struct sockaddr_in clnt_addr ;
	socklen_t clnt_addr_size ;

	char message[] = "Hellow World!" ;

	if(argc != 2)
	{
			printf("Usage : %s <port>\n", argv[0]) ;
			exit(1) ;
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0) ;
	if(serv_sock == -1) error_handling("Socket() error!\n") ;

	memset(&serv_addr, 0, sizeof(serv_addr)) ;
	serv_addr.sin_family = AF_INET ;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY) ;
	serv_addr.sin_port = htons(atoi(argv[1])) ;
	
	// IP주소와 PORT번호를 할당
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) error_handling("bind() error!\n") ;

	// 소켓을 연결요청을 받아들일 수 있는 상태로 바꿈 
	if(listen(serv_sock, 5) == -1) error_handling("listen() error!\n") ;
	
	clnt_addr_size = sizeof(clnt_addr) ;
	// 연결요청을 기다리는 accept()함수를 실행 -> 연결요청이 없을 경우 연결요청이 있을 때까지 함수는 반환하지 않음
	clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size) ;
	if(clnt_sock == -1) error_handling("accept() error!\n") ;

	// 데이터를 실제 전송하는 부분 -> 이 문장까지 실행되었다는 뜻은 연결요청이 들어왔고 정상적으로 연결된 상태라는 것
	write(clnt_sock, message, sizeof(message)) ;
	close(clnt_sock) ;
	close(serv_sock) ;
}

void error_handling(char *message)
{
		fputs(message, stderr) ;
		exit(1) ;
}

