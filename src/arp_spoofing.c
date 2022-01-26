#include "protocol.h"

enum {INDEX, HWADDR, ADDR};

int main(int argc, char **argv)
{
    int sock, len;
    struct sockaddr_ll sll;
    struct sockaddr_in *sin;
    struct ifreq ifr[3];
    struct etherhdr etherhdr;
    struct arphdr arphdr;
    uint8_t *buffer;
    uint8_t dst_mac[6], src_mac[6];
    struct in_addr target_ip, host_ip;
    
    if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0)
    {
        perror("socket ");
        return -1;
    }

    for(int i=0 ;i <3 ; i++)
    {
        memset(&ifr[i], 0x00, sizeof(struct ifreq));
        strcpy(ifr[i].ifr_name, IN_NAME);
    }

    if((ioctl(sock, SIOCGIFINDEX, &ifr[INDEX]))<0)
    {
        perror("ioctl index");
        return -1;
    }
    if((ioctl(sock, SIOCGIFHWADDR, &ifr[HWADDR]))<0)
    {
        perror("ioctl hwaddr");
        return -1;
    }
    if((ioctl(sock, SIOCGIFADDR, &ifr[ADDR]))<0)
    {
        perror("ioctl addr");
        return -1;
    }

    memcpy(src_mac, ifr[HWADDR].ifr_hwaddr.sa_data, 6);
    sin = (struct sockaddr_in *)&ifr[ADDR].ifr_addr;
    if(!inet_aton(argv[1],&target_ip))
    {
        printf("Target IP error\n");
        return -1;
    }
    if(!inet_aton(argv[2],&host_ip))
    {
         printf("Host IP error\n");
         return -1;
    }
    buffer = make_arp_request_packet(src_mac, sin->sin_addr, host_ip);

    memset(&sll, 0, sizeof(sll));
    sll.sll_ifindex = ifr[INDEX].ifr_ifindex;
    sll.sll_halen = ETH_ALEN; // length of destination mac address
    memset(sll.sll_addr, 0xFF, 6);

    if((len = sendto(sock, buffer, ARPMAX,0,(struct sockaddr*)&sll,sizeof(sll)))<0)
        perror("sendto");   

    printf("send arp packet\n");     

    return 0;
}