#include "protocol.h"

uint8_t* make_arp_request_packet(uint8_t source_mac[6], struct in_addr source_ip, struct in_addr target_ip)
{
    struct etherhdr *etherhdr;
    struct arphdr *arphdr;
    static uint8_t buffer[ARPMAX];

    arphdr->ar_hrd = htons(0x0001);
    arphdr->ar_pro = htons(0x0800);
    arphdr->ar_hln = 0x06;
    arphdr->ar_pln = 0x04;
    arphdr->ar_op = htons(0x0001);

    memcpy(arphdr->ar_sha, source_mac, 6);
    arphdr->ar_sip = source_ip.s_addr;
    memset(arphdr->ar_tha,0x00,6);
    arphdr->ar_tip = target_ip.s_addr;

    
    memset(etherhdr->ether_dhost, 0xff, 6);
    memcpy(etherhdr->ether_shost, source_mac ,6);
    etherhdr->ether_type = 0x0806;

    memcpy(buffer,etherhdr,sizeof(struct etherhdr));
    memcpy(buffer+sizeof(struct etherhdr), arphdr, sizeof(struct arphdr));

    return buffer; 
}