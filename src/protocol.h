#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <net/ethernet.h>

#include "pcap.h"

#define PACKMAX 4096
#define ETHMAX 1514
#define ARPMAX 42
#define IF_NAME "eth0"
#define ETHERTYPE_IP 0x0800
#define PSEUDO_HEADER_LEN 12

struct nic_info
{
    uint8_t my_mac[6];
    struct in_addr in_addr;
    struct in_addr broadaddr;
    struct in_addr maskaddr;
    int ifindex;
};

#ifndef __linux__
    #pragma pack(push,1)
#endif
struct etherhdr
{
	uint8_t  ether_dhost[6];		/* destination eth addr	*/
	uint8_t  ether_shost[6];		/* source ether addr	*/
	uint16_t ether_type;		/* packet type ID field	*/
}
#ifndef __linux__
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

#ifndef __linux__
    #pragma pack(push ,1)
#endif
struct	arphdr 
{
	uint16_t ar_hrd;		// Format of hardware address 
   	uint16_t ar_pro;		// Format of protocol address
   	uint8_t ar_hln;			// Length of hardware address 
   	uint8_t ar_pln;			// Length of protocol address 
   	uint16_t ar_op;			// ARP opcode (command) 
   	uint8_t ar_sha[6];		// Sender hardware address 
   	uint32_t ar_sip;		// Sender IP address why uint32_t? becouse sin_addr.s_addr is 4 byte 
   	uint8_t ar_tha[6];		// Target hardware address 
   	uint32_t ar_tip;		// Target IP address 
}
#ifndef __linux__
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

#ifndef __linux__
    #pragma pack(push, 1);
#endif
struct iphdr
{
    uint8_t ip_hl:4;		/* header length */
	uint8_t ip_v:4;		/* version */
	uint8_t ip_tos;			/* type of service */
	uint16_t ip_len;			/* total length */
	uint16_t ip_id;			/* identification */
	uint16_t ip_off;			/* fragment offset field */
#define	IP_RF 0x8000			/* reserved fragment flag */
#define	IP_DF 0x4000			/* dont fragment flag */
#define	IP_MF 0x2000			/* more fragments flag */
#define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
	uint8_t ip_ttl;			/* time to live */
	uint8_t ip_p;			/* protocol */
	uint16_t ip_sum;			/* checksum */
	struct in_addr ip_src, ip_dst;	/* source and dest address */
}
#ifndef __linux
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

typedef uint32_t tcp_seq;

#ifndef __linux__
    #pragma pack(push, 1);
#endif
struct tcphdr
{
	uint16_t th_sport;
    uint16_t th_dport;
    tcp_seq th_seq;
    tcp_seq th_ack;
    uint8_t th_x2:4;
    uint8_t th_off:4;
    uint8_t th_flags;
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
    uint16_t th_win;
    uint16_t th_sum;
    uint16_t th_urp;

}
#ifndef __linux
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

#ifndef __linux__
    #pragma pack(push, 1);
#endif
struct pseudohdr
{
    struct in_addr ip_src, ip_dst;
    uint8_t reserved;
    uint8_t protocol_type;
    uint16_t tcp_total_lenght;
}
#ifndef __linux
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

#ifndef __linux__
    #pragma pack(push, 1);
#endif
struct tcp_packet
{
    struct iphdr iphdr;
    struct tcphdr tcphdr;
}
#ifndef __linux
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

#ifndef __linux__
    #pragma pack(push, 1);
#endif
struct tcp_cksum_hdr
{
    struct pseudohdr pseudohdr;
    struct tcphdr tcphdr;
}
#ifndef __linux
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

#ifndef __linux__
    #pragma pack(push, 1);
#endif
struct icmphdr
{
    uint8_t  icmp_type;
	uint8_t  icmp_code;
	uint16_t icmp_cksum;
	uint16_t icmp_id;
	uint16_t icmp_seq;
}
#ifndef __linux
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

#ifndef __linux__
    #pragma pack(push, 1);
#endif
struct icmp_packet
{
    struct icmphdr icmp;
	uint8_t data[10];
}
#ifndef __linux
    ;
    #pragma pack(pop)
#else
__attribute__((__packed__));
#endif

int bentool_main(int argc, char **argv);
void usage(void);
int ping_scan(int args, ...);
int tcp_half_scan(int argc, char **argv);
uint8_t* make_arp_request_packet(uint8_t source_mac[6], struct in_addr source_ip, struct in_addr target_ip);
uint8_t* make_arp_reply_packet(uint8_t source_mac[6], struct in_addr source_ip, uint8_t target_mac[6], struct in_addr target_ip);
int tcp_half_scan(int argc, char **argv);
int get_interface_devices(char * arg);
int relay(uint8_t *dst_mac, char * if_name);
uint16_t cksum(void *data, uint32_t len);
int get_info(struct nic_info *nic_info, char *if_name);
int arp_spoof(char *i_if_name, char *i_target_ip, char *i_host_IP);
void make_tcp_header(struct tcp_packet *packet, const char *src_ip, uint16_t src_port, const char *dst_ip, uint16_t dst_port, uint32_t seq, uint32_t ack, uint8_t flag);
void make_ip_header(struct iphdr *iphdr, const char *src_ip, const char *dst_ip, uint16_t datalen);
void make_tcp_header_v2(struct tcphdr *packet, struct in_addr src_ip, uint16_t src_port, struct in_addr dst_ip, uint16_t dst_port, uint32_t seq, uint32_t ack, uint8_t flag);
void make_ip_header_v2(struct iphdr *iphdr, struct in_addr src_ip, struct in_addr dst_ip, uint16_t datalen);
int syn_flood(int argc, ...);
//int ping_scan(char *input_IP);
#endif
