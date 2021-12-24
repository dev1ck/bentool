#include <stdio.h>  //표준 입출력 라이브러리
#include <stdlib.h> // 문자열 변환, 난수 생성 , 동적 메모리 관리용 표준 라이브러리
#include <string.h> // 문자열을 다루는 함수를 포함한 라이브러리
#include <unistd.h> // 유닉스 환경에서 사용하는 C 컴파일러 헤더 파일 
#include <arpa/inet.h> // 숫자로 IP 주소를 조작하는 기능 정의 
#include <sys/socket.h> // BSD 소켓의 핵심 함수와 데이터 구조

void error_message(char *message); // char *message 변수를 매개변수로 갖는 void 형식으로 함수 error_message 선언

int main(int argc, char *argv[]) //정수 argc 와 명령어를 저장하는 *argv 배열을 매개변수로 갖는 main 함수 정의
{
	// ss : server_socket, cs : client_socket
	int ss, cs; // ss , cs 변수 선언
	
	char message[] = "Hello World!"; //message 배열에 helloworld! 저장

	// sa : server_address, ca : client_address
	struct sockaddr_in sa, ca; // sockaddr_in 구조체 sa, ca 변수 선언  

	// cas : client_address_size
	socklen_t cas;  // accept 의 3 번째 인자 cas

	if(argc != 2) // argc  즉, 입력한 명령어를 공백으로 구분. 2 개가 아닐경우 실행
	{
		printf("Usage : %s <Port>\n", argv[0]);  // argv 의 0 번 즉 server 실행시 썼던 명령어의 가장 앞부분 출력 
		exit(1); // 코드 종료 
	}

	ss = socket(PF_INET, SOCK_STREAM, 0);	 // socket함수를 이용. PF_INET 이라는 도메인 주소 , SOCK_STREAM 이라는 타입 , 0 프로토콜 번호를 인자로 넣어줌 그리고 SS 변수에 저장

	memset(&sa, 0, sizeof(sa)); // sa를 0으로 초기화 해 줌 
	sa.sin_family = AF_INET; // sa.sin_family에 주소체계인 AF_INET 값을 저장해줌 
	sa.sin_addr.s_addr = htonl(INADDR_ANY);   //a.sin_addr 안의 s_addr 에 시스템에 있는 모든 NIC 로 부터 요청을 받아들여, 호스트 바이트 순서를 빅 엔디안 바이트 순서로 변환 하여 저장.
	sa.sin_port = htons(atoi(argv[1]));  // sa.sin_port에 argv[1] 즉, 명령어 두번째 문자 값을 정수형으로 주소 변환하여 빅엔디안 방식으로 저장 

	bind(ss, (struct sockaddr*)&sa, sizeof(sa)); // bind 함수를 이용하여 소켓의 주소 지정 

	listen(ss, 5); // listen 함수를 이용하여 요청 대기 상태로 전환
	
	cas = sizeof(ca);  // ca의 길이를 cas변수에 저장 
	cs = accept(ss, (struct sockaddr*)&ca, &cas); //cs에 accept 함수를 사용하여 소켓 주소를 저장 

	write(cs, message, sizeof(message)); //cs에 저장된 소켓에서 message 의 크기(바이트) 만큼을 message에 저장
	close(cs); //cs 소켓을 종료시킴
	close(ss); //ss 소켓을 종료시킴
	return 0;
}

void error_message(char *message) // error_message 함수 정의
{
	fputs(message, stderr); // message 의 문자열을 stderr 에 씀 
	fputc('\n', stderr); // stderr 의 뒤에 \n 을 넣어줌
	exit(1);
}
