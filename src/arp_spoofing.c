#include "protocol.h"

enum {INDEX, HWADDR, ADDR};

int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_ll sll;
    struct sockaddr_in *sin;
    struct ifreq ifr[3];
    struct etherhdr etherhdr;
    struct arphdr arphdr;
    uint8_t buffer[ARPMAX];
    uint8_t dst_mac[6], src_mac[6];
    
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

    memcpy(src_mac, ifr[HWADDR].ifr_hwaddr.sa_data);
    sin = (struct sockaddr_in *)&ifr[ADDR].ifr_addr;


    //buffer = make_arp_request_packet();
    return 0;
}