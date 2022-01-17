#ifndef ARP_H // arp 헤더가 정의 되지 않았을 경우 arp 헤더를 정의해줌 
#define ARP_H

#include <stdint.h> // 자료형 정의 헤더 파일.

#define	ARPOP_REQUEST 1		/* ARP request.  */
#define	ARPOP_REPLY	2		/* ARP reply.  */ 
// ( 0001 은 request , 0002 는 reply)

#ifndef __linux__  // 리눅스가 아닐 경우 
	#pragma pack(push, 1) // 구조체의 기준 바이트를 1 로 만든다. -> 빈공간 없이 차곡 차곡 저장한다.
#endif
struct arphdr // arp 헤더의 type과 주소의 길이, operatrion 을 저장하는 구조체
{
	uint16_t ar_hrd;		/* Format of hardware address.  */
	uint16_t ar_pro;		/* Format of protocol address.  */
	uint8_t ar_hln;		/* Length of hardware address.  */
	uint8_t ar_pln;		/* Length of protocol address.  */
	uint16_t ar_op;		/* ARP operatrion code (command).  */
}
#ifndef __linux__
	;
	#pragma pack(pop)// 구조체 기준 1바이트 해제. 차곡차곡 저장하던걸 기본으로 돌려놓음 
#else
__attribute__ ((__packed__)); // 구조체 바이트 빈공간 제어 -> 빈공간 없이 차곡차곡 저장 
#endif

#ifndef __linux__
	#pragma pack(push, 1) // 구조체 기준 바이트 1 로 설정
#endif
struct	ether_arp // 샌더와 목적지 arp 헤더 주소를 저장하는 구조체
{
	struct	arphdr ea_hdr;		/* fixed-size header */

	uint8_t arp_sha[6];	/* sender hardware address */
	uint8_t arp_spa[4];	/* sender protocol address */
	uint8_t arp_tha[6];	/* target hardware address */
	uint8_t arp_tpa[4];	/* target protocol address */
}
#ifndef __linux__
	;
	#pragma pack(pop) // 구조체 기준 1바이트 해제
#else
__attribute__ ((__packed__)); // 구조체 바이트 빈공간 제어 -> 빈공간 없이 차곡차곡 저장
#endif

#define	arp_hrd	ea_hdr.ar_hrd // 구조체 변수 arp_hrd 선언 - 맥주소 
#define	arp_pro	ea_hdr.ar_pro // 구조체 변수 arp_pro 선언 - 프로토콜 주소 
#define	arp_hln	ea_hdr.ar_hln // 구조체 변수 arp_hln 선언 - 맥주소 길이 
#define	arp_pln	ea_hdr.ar_pln // 구조체 변수 arp_pln 선언 - 프로토콜 주소길이
#define	arp_op	ea_hdr.ar_op // 구조체 변수 arp_op 선언 - 오퍼레이트, 요청 패킷인지 응답 패킷인지 결정

void dump_arp_header(struct ether_arp *arp); // 목적지와 샌더의 주소를 갖는 구조체 변수 arp 를 선언하는 함수 dump_arp_header 선언

#endif