#include <stdio.h> //표준 읿출력 라이브러리
#include <stdlib.h> // 문자열 변환, 난수생성, 동적 메모리 관리용 표준 라이브러리
#include <string.h> // 문자열을 다루는 함수를 포함한 라이브러리 
#include <unistd.h> // 유닉스 환경에서 사용하는 C 컴파일러 라이브러리
#include <fcntl.h> // 열려진 파일의 속성을 가져오거나 설정
#include <arpa/inet.h> // 숫자로 IP 주소를 조작하는 기능 정의 
#include <sys/socket.h> // BSD 소켓의 핵심 함수와 데이터 구조

void error_message(char *message); // void 형식의 *message 를 매개변수로 갖는 error_message 함수 선언 

int main(int argc, char *argv[]) // int 형식의 argc ,  *argv[] 를 매개 변수로 갖는 main  함수 선언
{
	int sock, str_len; // sock, str_len 선언
	char message[30]; // message 배열을 선언하고, 배열 크기를 설정 
	//sa : server_address
	struct sockaddr_in sa; //sockaddr_in 구조체 sa 변수를 선언
	
	if(argc != 3) // argc 즉, 공백을 구분자로 명령어가 3 개가 아닐 경우 
	{
		printf("Usage : %s <IP> <Port>\n", argv[0]); // 명령어의 가장 앞 부분 출력
		exit(1); // 코드 종료
	}

	sock = socket(PF_INET, SOCK_STREAM, 0); // socket 함수를 sock 변수에 저장

	memset(&sa, 0, sizeof(sa)); // sa 를 0 으로 초기화 해줌 
	sa.sin_family = AF_INET; // sa구조체 안의 sin_family 에 AF_INET 주소를 삽입 
	sa.sin_addr.s_addr = inet_addr(argv[1]); //ip 주소를 inet_addr() 이용하여 네트워크 순서를 갖는 이진 바이너리로 변환 
	sa.sin_port = htons(atoi(argv[2])); // sa.sin_port에 argv[2] 즉, 명령어 세번째 문자 값을 정수형으로 주소 변환하여 빅엔디안 방식으로 저장                        시스템에서 알아먹을수 있도록 정수형으로 변환

	connect(sock, (struct sockaddr*)&sa, sizeof(sa)); // connect함수를 이용하여 sock에 담긴 소켓, sa 에 담긴 서버의 주소로 연결

	read(sock, message, sizeof(message)); // sock으로부터 message 의 크기(바이트) 만큼 읽어서 message에 저장함

	printf("Message from server : %s \n", message); // 서버에서 읽은 message를 
	close(sock); // sock 을 중지한다.
	return 0;
}

void error_message(char *message) // 에러를 띄우는 error_message 함수를 정의
{
	fputs(message, stderr); // message 의 문자열을 stderr 에 씀 
	fputc('\n', stderr); // stderr 의 뒤에 \n 을 넣어줌
	exit(1); // 코드 종료
}
