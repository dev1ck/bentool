#include "protocol.h"

int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_ll sll;
    struct etherhdr etherhdr;
    struct arphdr arphdr;
    uint8_t buffer[ARPMAX];
    
    if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0)
    {
        perror("socket ");
        return -1;
    }
}

uint8_t* make_arp_request_packet(uint8_t my_mac[6], struct in_addr my_ip, struct in_addr target_ip)
{
    struct etherhdr *etherhdr;
    struct arphdr *arphdr;
    static uint8_t buffer[ARPMAX];

    arphdr->ar_hrd = htons(0x0001);
    arphdr->ar_pro = htons(0x0800);
    arphdr->ar_hln = 0x06;
    arphdr->ar_pln = 0x04;
    arphdr->ar_op = htons(0x0001);

    memcpy(arphdr->ar_sha, my_mac, 6);
    arphdr->ar_sip = my_ip.s_addr;
    memset(arphdr->ar_tha,0x00,6);
    arphdr->ar_tip = target_ip.s_addr;

    
    memset(etherhdr->ether_dhost, 0xff, 6);
    memcpy(etherhdr->ether_shost, my_mac ,6);
    etherhdr->ether_type = 0x0806;

    memcpy(buffer,etherhdr,sizeof(struct etherhdr));
    memcpy(buffer+sizeof(struct etherhdr), arphdr, sizeof(struct arphdr));

    return buffer; 
}

uint8_t* make_arp_reply_packet(uint8_t my_mac[6], struct in_addr spoof_ip, uint8_t target_mac[6], struct in_addr target_ip)
{
    struct etherhdr *etherhdr;
    struct arphdr *arphdr;
    static uint8_t buffer[ARPMAX];

    arphdr->ar_hrd = htons(0x0001);
    arphdr->ar_pro = htons(0x0800);
    arphdr->ar_hln = 0x06;
    arphdr->ar_pln = 0x04;
    arphdr->ar_op = htons(0x0002);

    memcpy(arphdr->ar_sha, my_mac, 6);
    arphdr->ar_sip, spoof_ip.s_addr;
    memcpy(arphdr->ar_tha, target_mac ,6);
    arphdr->ar_tip = target_ip.s_addr;

    
    memcpy(etherhdr->ether_dhost, target_mac, 6);
    memcpy(etherhdr->ether_shost, my_mac ,6);
    etherhdr->ether_type = 0x0806;

    memcpy(buffer,etherhdr,sizeof(struct etherhdr));
    memcpy(buffer+14, arphdr, sizeof(struct arphdr));

    return buffer; 
}