#ifndef _TCPLIB_H
#define _TCPLIB_H

#include <stdint.h>

#define ETHERTYPE_IP 0x0800

// #ifndef __linux__
//     #pragma pack(push,1)
// #endif
struct ether_header
{
    uint8_t ether_dhost[6];
    uint8_t ether_shost[6];
    uint16_t ether_type;
};
// #ifndef __linux__
//     ;
//     #pragma pack(pop)
// #else
// __attribute__((__packed__));
// #endif

// #ifndef __linux__
//     #pragma pack(push ,1)
// #endif
struct ip
{
    uint8_t ip_hl:4;
    uint8_t ip_v:4;
    uint8_t ip_tos;
    uint16_t ip_len;
    uint16_t ip_id;
    uint16_t ip_off;
#define IP_RF 0x8000
#define IP_DF 0x4000
#define IP_MF 0x2000
#define IP_OFFMASK 0x1fff
    uint8_t ip_ttl;
    uint8_t ip_p;
    uint16_t ip_sum;
    struct in_addr ip_src, ip_dst;
};
// #ifndef __linux
//     ;
//     #pragma pack(pop)
// #else
// __attribute__((__packed__));
// #endif

typedef uint32_t tcp_seq;

// #ifndef __linux__
//     #pragma pack(push, 1);
// #endif
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
};
// #ifndef __linux__
//     ;
//     #pragma pack(pop)
// #else
// __attribute__((__packed__));
// #endif

// #ifndef __linux__
//     #pragma pack(push, 1)
// #endif
struct tcp_packet
{
    struct ether_header ether_header;
    struct ip iphdr;
    struct tcphdr tcphdr;
};
// #ifndef __linux__
//     ;
//     #pragma pack(pop)
// #else
// __attribute__((__packed__));
// #endif

void make_tcp_header(struct tcp_packet *packet, const char *src_ip, uint16_t src_port, 
const char *dst_ip, uint16_t dst_port, 
uint32_t seq, uint32_t ack, uint8_t flag);

void make_ip_header(struct ip *iphdr, const char *src_ip, const char *dst_ip, uint16_t datalen);

uint16_t tcp_cksum(uint16_t *data, uint32_t len);
void  strmac_to_buffer(const char *str, uint8_t *mac);
#endif