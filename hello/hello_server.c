#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* Success : file descriptor || Fail : -1 
	<소켓을 쓰기위해서 기본적으로 생성하는 함수>
	int socket(int domain, int type, int protocol) ;
	
	1. domain : 소켓이 사용할 프로토콜 체계(Protocol Family) 전달
		- Protocol Family
			1. PF_INET : IPv4 인터넷 프로토콜 체계 -> 주로 사용!!
			2. PF_INET6 : IPv6 인터넷 프로토콜 체계
			3. PF_LOCAL : 로컬 통신을 위한 UNIX 프로토콜 체계
			4. PF_PACKET : Low Level 소켓을 위한 프로토콜 체계
			5. PF_IPX : IPX 노벨 프로토콜 체계
	2. type : 소켓의 데이터 전송방식에 대한 정보 전달
		- 두 가지 전송방식이 존재함
			1. SOCK_STREAM(연결지향형 소켓) - "신뢰성 있는 순차적인 바이트 기반의 연결지향 데이터 전송 방식의 소켓"
				- 전송 순서대로 데이터가 수신된다.
				- 중간에 데이터가 소멸되지 않고 목적지로 전송된다.
				- 전송되는 데이터의 경계가 존재하지 않는다.
					-> 전송된 데이터는 일단 buffer에 저장되고 read()를 이용하여 여러번에 걸쳐 buffer에서 읽어올 수도 있고 한번에 읽어 올 수도 있다. 따라서 데이터의 경계가 존재하지 않는다고 말한다.
					-> buffer가 꽉참으로 인해 데이터 손실이 발생할 경우는 거의 없다. buffer가 꽉 찰 경우 애초에 소켓에서 수신을 할 수 없는 상태가 된다.
					-> 즉, 연결지향형 소켓은 자신과 연결된 상대 소켓의 상태를 파악해가면서 데이터를 전송한다. 
				- 소켓의 연결의 1:1의 관계이다.
			2. SOCK_DGRAM(비 연결지향형 소켓)
				- 전송된 순서에 상관없이 가장 빠른 전송을 지향한다. - "신뢰성과 순차적 데이터 전송을 보장하지 않는, 고속의 데이터 전송을 목적으로 하는 소켓"
				- 전송된 데이터는 손실의 우려가 있고, 파손의 우려가 있다.
				- 전송되는 데이터의 경계가 존재한다.
					-> 한번에 전송할 수 있는 데이터 크기가 제한되기 때문에 한번에 크기가 큰 데이터를 보낼 때 나누어서 보내야만 한다. (데이터의 경계가 존재한다.)
					-> 즉, 데이터를 전송할 때 두 번의 함수 호출이 있었다면 데이터를 수신할 때에도 두 번의 함수 호출이 있어야만 한다. 
				- 한번에 전송할 수 있는 데이터의 크기가 제한된다.
	3. protocol : 두 컴퓨터간 통신에 사용되는 프로토콜 정보 전달
		- 사실 첫번째 두번째 인자만으로도 충분히 소켓을 생성할 수 있다. 하지만 다음과 같은 예외 때문에 이 인자가 필요하다.
			-> "하나의 프로토콜 체계안에 데이터의 전송방식이 동일한 프로토콜이 둘 이상 존재하는 경우"
			-> 즉, 프로토콜 체계와 전송방식이 둘 다 같고 다만 다른 프로토콜 두 개를 필요로 한다면 세번째 인자를 사용하여 달리 선언할 수 있다.
				ex) socket(PF_INET, SOCK_STREAM, IPPROTO_TCP1) ;
					socket(PF_INET, SOCK_STREAM, IPPROTO_TCP2) ;
					** 1, 2번째 인자는 모두 같지만 마지막 인자만 다르다.**

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

