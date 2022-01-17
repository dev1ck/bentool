#include <stdio.h> // 기본 입출력 라이브러리
#include <string.h> // 문자열을 다루기 위한 라이브러리 

//#ifndef __linux__
//	#include <winsock2.h> // 윈도우 소켓 라이브러리
//#else
	#include <netinet/in.h> // IPv4 전용 기능 라이브러리
	#include <arpa/inet.h> // 주소 조작,변환 라이브러리 
//#endif

#include "./arp.h" // arp.h 파일을 포함 

int main(void)
{
	struct ether_arp ether_arp; // mac주소, 프로토콜 주소, 길이 를 포함하는 구조체 변수 ether_arp 선언 
	int src_ip, dst_ip; // 소스 ip 변수 , 목적지 ip 변수 선언 
	int i; //반복을 위한 i 변수 선언 

	ether_arp.arp_hrd = htons(1); // 1 (이더넷 ) int 형 데이터를 네트워크 순서로 변환해 mac 주소에 저장 
	ether_arp.arp_pro = htons(0x0800); // 0x0800(ipv4) int 형 데이터를 네트워크 순서로 변환해 protocol 주소에 저장
	ether_arp.arp_hln = 6; // mac 주소의 크기를 6byte 로 설정 
	ether_arp.arp_pln = 4; // protocol 주소의 크기를 4byte 로 설정 
	ether_arp.arp_op = htons(ARPOP_REQUEST); // arp.h 안의 arp request 값인 1 을 네트워크 순서로 변환해 arp_op에 저장 ( 0001 은 request , 0002 는 reply)

<<<<<<< HEAD
	src_ip = inet_addr("192.168.214.131");
	dst_ip = inet_addr("5.6.7.8");
	printf("\n src_ip : %d \n\n",src_ip);
	printf("\n dst_ip : %d\n\n",dst_ip);
	memcpy(ether_arp.arp_spa, &src_ip, 4);
	memcpy(ether_arp.arp_tpa, &dst_ip, 4);
	printf("\n arp_spa : %d \n\n",ether_arp.arp_spa);
	printf("\n arp_tpa : %d \n\n",ether_arp.arp_tpa);
	for(i = 0; i < 6; i += 1) {
		ether_arp.arp_sha[i] = i;
=======
	src_ip = inet_addr("1.2.3.4"); // 문자형 샌더 주소 정수형으로 src_ip 에 저장 
	dst_ip = inet_addr("5.6.7.8"); // 문자형 목적지 주소를 정수형으로 dst_ip 에 저장 

	memcpy(ether_arp.arp_spa, &src_ip, 4); // src_ip 의 프로토콜 주소를 4byte 크기만큼 arp_spa 에 저장 
	memcpy(ether_arp.arp_tpa, &dst_ip, 4); // dst_ip 의 목적지 프로토콜 주소를 4byte 크기만큼 arp_tpa 에 저장

	for(i = 0; i < 6; i += 1) { // 6번 반복 
		ether_arp.arp_sha[i] = i; // sender mac 주소를 0~6 까지 저장 -> 1,2,3,4,5,6
>>>>>>> 6e8ba99ccd84039da267ecff27de4c84492446fb
	}

	for(i = 0; i < 6; i += 1) {
		ether_arp.arp_tha[i] = i + 0x10; // target mac 주소를 0~6 까지 저장 
	}

	printf("ether_arp : %lu\n", (unsigned long)sizeof(struct ether_arp)); // ether_arp구조체의 크기를 출력

	dump_arp_header(&ether_arp); // ether_arp 구조체 변수 arp 에 ether_arp의 내용을 저장 

	return 0;
}
