#include "protocol.h"
#define STRSIZE 1024
#define BUFSIZE 4096

enum {ARGV_CMD, ARGV_INTERFACE, ARGV_IP, ARGV_PORT};

int main(int argc, char **argv)
{
    init_tcp_connection(argc, argv);
    return 0;
}

int init_tcp_connection(int argc, char **argv)
{
    int sock, len, filter_port, pos;
    struct sockaddr_ll sll;
    struct ifreq ifr;
    unsigned int filter_ip;
    unsigned char buf[BUFSIZE];
    char str_sip[STRSIZE], str_dip[STRSIZE];
    char flags[STRSIZE];

    if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    {
        perror("socket ");
        return 1;
    }

    memset(&ifr, 0x00, sizeof(ifr));

    strcpy(ifr.ifr_name, argv[ARGV_INTERFACE]);
    if(ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("ioctl ");
        return 1;
    }

    memset(&sll, 0x00, sizeof(sll));
    sll.sll_family = PF_PACKET;
    sll.sll_ifindex = ifr.ifr_index;
    sll.sll_protocol = htons(ETH_P_ALL);

    if(bind(sock, (struct sockaddr *)&sll, sizeof(sll)) < 0)
    {
        perror("bind ");
        return 1;
    }

    filter_ip = inet_addr(argv[ARGV_IP]);
    filter_port = htons(atoi(argv[ARGV_PORT]));

    while((len = read(sock, buf, BUFISZE)) >= 0)
    {
        struct iphdr *ip = NULL;
        struct tcphdr *tcp = NULL;
        struct ehter_header *ether_header = (struct ether_header*)buf;

        if(ntohs(ether_header->ether_type) != ETHERTYPE_IP) continue;

        ip = (struct iphdr *)(buffer + sizeof(struct ether_header));

        if((ip->ip_p == IPPROTO_TCP) && ((ip->src_ip.s_addr == filter_ip) || (ip->dst_ip.s_addr == filter_ip)))
        {
            tcp = (struct tcphdr *)((char *)ip + (ip->ip_hl<<2));

            if(tcp->th_dport == filter_port || tcp->th_sport == filter_port)
            {
                pos = 0;

                if((tcp->th_flags & TH_SYN) == TH_SYN) flags[pos++] = 'S';
                if((tcp->th_flags & TH_FIN) == TH_FIN) flags[pos++] = 'F';
                if((tcp->th_flags & TH_RST) == TH_RST) flags[pos++] = 'R';
                if((tcp->th_flags & TH_PUSH) == TH_PUSH) flags[pos++] = 'P';
                if((tcp->th_flags & TH_ACK) == TH_ACK) flags[pos++] = 'A';
                if((tcp->th_flags & TH_URG) == TH_URG) flags[pos++] = 'U';

                flags[pos] = '\0';

                strcpy()

            }
        }
    }

}