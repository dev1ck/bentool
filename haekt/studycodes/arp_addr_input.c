#include <stdio.h> // 기본 입출력 라이브러리
#include <string.h> // 문자열을 다루기 위한 라이브러리

#ifndef __linux__
	#include <winsock2.h> // 윈도우 소켓 라이브러리 
#else
	#include <netinet/in.h> // IPv4 전용 기능 라이브러리
	#include <arpa/inet.h> // 주소 조작, 변환 라이브러리 
#endif

#include "arp.h"

void dump_arp_header(struct ether_arp *arp) // 주소를 넣기위한 함수 
{
	char src_ip[1024], dst_ip[1024]; // 샌더 주소와 목적지 주소를 담을 1024 byte 크기의 char 형 배열 선언 

	unsigned char *s = arp->arp_sha; // 샌더 주소를 char 형 s포인터 변수에 저장 
	unsigned char *t = arp->arp_tha; // 목적지 주소를 char 형 t포인터 변수에 저장 

	struct in_addr ina_src_ip, ina_dst_ip; // 32bit ip 주소를 담는 in_addr 구조체 변수 ina_src_ip, ina_dst_ip 를 선언 

	memcpy(&(ina_src_ip.s_addr), arp->arp_spa, 4); // arp_spa 의 4byte 만큼의 샌더의 ip 주소를 ina_src_ip 에 복사해 저장 
	sprintf(src_ip, "%s", inet_ntoa(ina_src_ip)); // ina_src_ip에 저장된 정수형 ip값을 문자형(. 닷형) 으로 바꾸어 src_ip 에 저장한다.

	memcpy(&(ina_dst_ip.s_addr), arp->arp_tpa, 4); // arp_tpa 의 4byte 만큼의 목적지 ip 주소를 ina_dst_ip 에 복사해 저장 
	sprintf(dst_ip, "%s", inet_ntoa(ina_dst_ip)); // ina_dst_ip 에 저장된 정수형 ip 주소를 문자형(. 닷형) 으로 바꾸어 dst_ip 에 저장한다.

<<<<<<< HEAD
	printf("[ARP][OP:%d][%02x:%02x:%02x:%02x:%02x:%02x->"  // Op : request 인지, reply 인지 출력 / 샌더의 맥 주소와 목적지 맥주소 출력 / 샌더 -> 목적지 ip 출력 
=======
	printf("[ARP][OP:%d][%02x:%02x:%02x:%02x:%02x:%02x->" // Op : request 인지, reply 인지 출력 / 샌더의 맥 주소와 목적지 맥주소 출력 / 샌더 -> 목적지 ip 출력 
>>>>>>> 6e8ba99ccd84039da267ecff27de4c84492446fb
		"%02x:%02x:%02x:%02x:%02x:%02x][%s->%s]\n", 
		ntohs(arp->arp_op), 
		s[0], s[1], s[2], s[3], s[4], s[5],
		t[0], t[1], t[2], t[3], t[4], t[5],		
		src_ip, dst_ip
	);		
}
<<<<<<< HEAD

=======
>>>>>>> 6e8ba99ccd84039da267ecff27de4c84492446fb
