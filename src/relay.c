#include "protocol.h"

int main(int argc, char * argv[])
{  
    int sock, len, rlen;
    unsigned char buffer[ETHMAX]={0,};
    struct sockaddr_ll sll, sadr_ll;
    struct sockaddr_in * my_addr;
    struct etherhdr* eth_h;
    struct iphdr *ip_header;
    struct ifreq ifr[3];
    void *iphdr_ptr;
    unsigned char my_mac[6];
    unsigned char dst_mac[6];
    char * ptr;
    char p_src[16], p_dst[16];

    int i = 0;

    ptr = strtok(argv[1],":");
    while(ptr != NULL)
    {
        dst_mac[i]=(char)strtol(ptr, NULL, 16);
        ptr = strtok(NULL, ":");
        i++;
    }

    if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0)
    {
        perror("socket");
        return -1;
    }
    
    for(int i=0 ;i <3 ; i++)
    {
        memset(&ifr[i], 0x00, sizeof(struct ifreq));
        strcpy(ifr[i].ifr_name, "eth0");
    }

    ioctl(sock, SIOCGIFINDEX, &ifr[0]);
    ioctl(sock, SIOCGIFHWADDR, &ifr[1]);
    ioctl(sock, SIOCGIFADDR, &ifr[2]);
    
    memcpy(my_mac, ifr[1].ifr_hwaddr.sa_data, 6);
    my_addr = (struct sockaddr_in *)&ifr[2].ifr_addr;

    memset(&sll, 0x00, sizeof(sll));
	sll.sll_family = PF_PACKET;
	sll.sll_ifindex = ifr[0].ifr_ifindex;
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
        
        if(ip_header->ip_src.s_addr != my_addr->sin_addr.s_addr &&
           ip_header->ip_dst.s_addr != my_addr->sin_addr.s_addr)
        {
            strcpy(p_src, inet_ntoa(ip_header->ip_src));
            strcpy(p_dst, inet_ntoa(ip_header->ip_dst));

            printf("%s -> %s\n",p_src, p_dst);

            memcpy(eth_h->ether_dhost,dst_mac,6);
            memcpy(eth_h->ether_shost, my_mac, 6);
            
            memset(&sadr_ll, 0, sizeof(sadr_ll));
            sadr_ll.sll_ifindex = ifr[0].ifr_ifindex;
            sadr_ll.sll_halen = ETH_ALEN; // length of destination mac address
            memcpy(sadr_ll.sll_addr, dst_mac, 6);

            if((len = sendto(sock, buffer, rlen,0,(struct sockaddr*)&sadr_ll,sizeof(sadr_ll)))<0)
                perror("sendto");
        }        
    }
    close(sock);
    return 0;
}
