#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
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

#define ICMPMAX 4096
#define ETHMAX 1514
#define ARPMAX 42

struct etherhdr
{
	uint8_t  ether_dhost[6];		/* destination eth addr	*/
	uint8_t  ether_shost[6];		/* source ether addr	*/
	uint16_t ether_type;		/* packet type ID field	*/
};

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
};

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
};

struct tcphdr
{

};

struct icmphdr
{
    uint8_t  icmp_type;
	uint8_t  icmp_code;
	uint16_t icmp_cksum;
	uint16_t icmp_id;
	uint16_t icmp_seq;
};

struct icmp_packet
{
	struct icmphdr icmp;
	char data[10];
};

int ping_scan(char *input_IP);