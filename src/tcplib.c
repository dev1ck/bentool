#include <stdio.h>

#ifndef __linux__
    #include <winsock2.h>
#else
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

#include "tcplib.h"

#define PSEUDO_HEADER_LEN 12

void make_tcp_header(struct tcp_packet *packet, const char *src_ip, uint16_t src_port, const char *dst_ip, uint16_t dst_port, uint32_t seq, uint32_t ack, uint8_t flag)
{
    packet->tcphdr.th_seq = htonl(seq);
    packet->tcphdr.th_ack = htonl(ack);
    packet->tcphdr.th_sport = ntohs(src_port);
    packet->tcphdr.th_dport = ntohs(dst_port);
    packet->tcphdr.th_off = 5;
    packet->tcphdr.th_flags = flag;
    packet->tcphdr.th_win = htons(8192);
    packet->tcphdr.th_urp = 0;

    packet->iphdr.ip_ttl = 0;
    packet->iphdr.ip_p = IPPROTO_TCP;
    packet->iphdr.ip_src.s_addr = inet_addr(src_ip);
    packet->iphdr.ip_dst.s_addr = inet_addr(dst_ip);
    packet->iphdr.ip_sum = ntohs(sizeof(packet->tcphdr));

    packet->tcphdr.th_sum = 0;
    packet->tcphdr.th_sum = tcp_cksum((unsigned short*)&(packet->iphdr.ip_ttl), PSEUDO_HEADER_LEN + sizeof(packet->tcphdr));
}

void make_ip_header(struct ip *iphdr, const char *src_ip, const char *dst_ip, uint16_t datalen)
{
    iphdr->ip_v = 4;
    iphdr->ip_hl = sizeof(struct ip) >> 2;
    iphdr->ip_id = 100;
    iphdr->ip_len = htons(sizeof(struct ip) + datalen);
    iphdr->ip_off = htons(0);
    iphdr->ip_ttl = 128;
    iphdr->ip_p = IPPROTO_TCP;
    iphdr->ip_src.s_addr = inet_addr(src_ip);
    iphdr->ip_dst.s_addr = inet_addr(dst_ip);
    iphdr->ip_sum = 0;
    iphdr->ip_sum = tcp_cksum((unsigned short *)iphdr, sizeof(struct ip));
}

uint16_t tcp_cksum(uint16_t *data, uint32_t len)
{
    unsigned long sum = 0;

    for(; 1 < len; len -= 2)
    {
        // sum = sum + *data
        // *data++
        sum += *data++;

        if(sum & 0x80000000)
            sum = (sum & 0xffff) + (sum >> 16);
    }

    if(len == 1)
    {
        unsigned short i = 0;
        *(unsigned char *)(&i) = *(unsigned char*)data;
        sum += i;
    }

    while(sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return (sum == 0xffff) ? sum : ~sum;
}

void strmac_to_buffer(const char *str, uint8_t *mac)
{
    int i;
    unsigned int tmac[6];

    sscanf(str, "%x:%x:%x:%x:%x:%x", &tmac[0], &tmac[1], &tmac[2], &tmac[3], &tmac[4], &tmac[5]);

    for(i = 0; i < 6; i += 1)
    {
        mac[i] = (unsigned char)tmac[i];
    }
}