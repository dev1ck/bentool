#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <errno.h>

#define IPv4_BUFFER 20
#define PREFIX_BUFFER 5

struct icmp_packet
{
	struct icmphdr icmp;
	char data[32];
};

void ping_scan(char *IP_addr);
uint16_t cksum(uint16_t *data, uint32_t len);
int send_ping(int sock, u_int32_t ip, struct icmp_packet icmp_packet);

