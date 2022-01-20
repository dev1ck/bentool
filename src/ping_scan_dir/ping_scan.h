#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

#define BUFMAX 4096

struct ip
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
struct icmp
{
	uint8_t  icmp_type;
	uint8_t  icmp_code;
	uint16_t icmp_cksum;
	uint16_t icmp_id;
	uint16_t icmp_seq;
};
struct icmp_packet
{
	struct icmp icmp;
	char data[10];
};

void ping_scan(char *IP_addr);
uint16_t cksum(uint16_t *data, uint32_t len);
int send_ping(int sock, u_int32_t ip, struct icmp_packet icmp_packet);
void *thread_function(void *p);
void receive_ping(int sock);

