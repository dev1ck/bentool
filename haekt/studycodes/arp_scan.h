#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<linux/if_packet.h> // sockaddr_ll
#include<linux/if_ether.h> 
#include<net/ethernet.h>
#include<stdlib.h>

#ifndef __linux__
    #include <winsock.h>
#else
    #include <netinet/in.h> // IPv4 전용 기능 라이브러리
	#include <arpa/inet.h> // 주소 조작,변환 라이브러리
#endif

#define ETH_SIZE 42
#define ARP_REQUEST 1
#define ARP_REPLY 2
#define ETH_IP 0x0800
#define ETH_ARP 0x0806

#ifndef __linux__  // 리눅스가 아닐 경우 
    #pragma pack(push, 1) // 구조체의 기준 바이트를 1 로 만든다. -> 빈공간 없이 차곡 차곡 저장한다.
#endif
struct eth_hdr{
   uint8_t dest_addr[6];	// Destination hardware address
   uint8_t src_addr[6];	// Source hardware address 
   uint16_t frame_type;		// Ethernet frame type 
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
struct arp_hdr {
   uint16_t ar_hrd;		// Format of hardware address 
   uint16_t ar_pro;		// Format of protocol address
   uint8_t ar_hln;		// Length of hardware address 
   uint8_t ar_pln;		// Length of protocol address 
   uint16_t ar_op;		// ARP opcode (command) 
   uint8_t ar_sha[6];	// Sender hardware address 
   uint32_t ar_sip;		// Sender IP address why uint32_t? becouse sin_addr.s_addr is 4 byte 
   uint8_t ar_tha[6];	// Target hardware address 
   uint32_t ar_tip;		// Target IP address 
}
#ifndef __linux__
    ;
    #pragma pack(pop) // 구조체 기준 1바이트 해제
#else
__attribute__ ((__packed__)); // 구조체 바이트 빈공간 제어 -> 빈공간 없이 차곡차곡 저장
#endif


