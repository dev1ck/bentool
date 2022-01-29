#include "protocol.h"

int relay(uint8_t *dst_mac, char *if_name)
{  
    int sock, len, rlen;
    unsigned char buffer[ETHMAX]={0,};
    struct sockaddr_ll sll, sadr_ll;
    struct nic_info info;
    struct etherhdr* eth_h;
    struct iphdr *ip_header;
    void *iphdr_ptr;
    char * ptr;

    int i = 0;

    if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0)
    {
        perror("socket");
        return -1;
    }
    
    get_info(&info, if_name);

    memset(&sll, 0x00, sizeof(sll));
	sll.sll_family = PF_PACKET;
	sll.sll_ifindex = info.ifindex;
	sll.sll_protocol = htons(ETH_P_ALL);

    if(bind(sock, (struct sockaddr *)&sll, sizeof(sll))<0)
    {
        perror("bind");
        return -1;
    }

    while(1)
    {
        if((rlen = read(sock, buffer, ETHMAX))<0)
            perror("read");

        iphdr_ptr = buffer + sizeof(struct etherhdr);
        eth_h = (struct etherhdr*)buffer;
        ip_header = (struct iphdr*)iphdr_ptr;
        
        if(ip_header->ip_src.s_addr != info.in_addr.s_addr &&
           ip_header->ip_dst.s_addr != info.in_addr.s_addr)
        {
            memcpy(eth_h->ether_dhost,dst_mac,6);
            memcpy(eth_h->ether_shost, info.my_mac, 6);
            
            memset(&sadr_ll, 0, sizeof(sadr_ll));
            sadr_ll.sll_ifindex = info.ifindex;
            sadr_ll.sll_halen = ETH_ALEN; // length of destination mac address
            memcpy(sadr_ll.sll_addr, dst_mac, 6);

            if((len = sendto(sock, buffer, rlen,0,(struct sockaddr*)&sadr_ll,sizeof(sadr_ll)))<0)
                perror("sendto");
        }        
    }
    close(sock);
    return 0;
}
