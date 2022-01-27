#include "protocol.h"

#define PSEUDO_HEADER_LEN 12

void make_tcp_header(struct tcp_packet *packet, const char *src_ip, uint16_t src_port, 
const char *dst_ip, uint16_t dst_port, 
uint32_t seq, uint32_t ack, uint8_t flag);
void make_ip_header(struct iphdr *iphdr, const char *src_ip, const char *dst_ip, uint16_t datalen);
void  strmac_to_buffer(const char *str, uint8_t *mac);

enum {ARGV_CMD, ARGV_GARBAGE_1, ARGV_GARBAGE_2, ARGV_GARBAGE_3, ARGV_GARBAGE_4, ARGV_MY_IP, ARGV_TARGET_IP, ARGV_START_PORT, ARGV_END_PORT};

void *tcp_thread_function(void *p);

struct param_data
{
    int sock;
    uint16_t start_port;
    uint16_t end_port;
};

int tcp_half_scan(int argc, char **argv)
{
    pthread_t thread_id;
    int on = 1;
    struct sockaddr_in addr;
    uint16_t start_port, end_port, port;
    struct param_data param;
    struct tcp_packet packet;

    if(argc<=5)
    {
        printf("Usage : %s [my ip] [target ip] [start port] [end port]\n", argv[ARGV_CMD]);
        return 1;
    }

    if((param.sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
    {
        perror("socket ");
        return 1;
    }

    if(setsockopt(param.sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
    {
        perror("setsockopt ");
        return 1;
    }
    param.start_port = start_port = (uint16_t)atoi(argv[ARGV_START_PORT]);
    param.end_port = end_port = (uint16_t)atoi(argv[ARGV_END_PORT]);

    pthread_create(&thread_id, NULL, tcp_thread_function, &param);

    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[ARGV_TARGET_IP]);

    memset(&packet, 0x00, sizeof(packet));

    

    for(port = start_port; port <= end_port; port += 1)
    {
        make_tcp_header(&packet, argv[ARGV_MY_IP], rand(), argv[ARGV_TARGET_IP], port, rand(), 0, TH_SYN);
        make_ip_header(&(packet.iphdr), argv[ARGV_MY_IP], argv[ARGV_TARGET_IP], sizeof(struct tcphdr));

        if(sendto(param.sock, &(packet.iphdr), sizeof(struct iphdr) + sizeof(struct tcphdr), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("sendto ");
            break;
        }
    }

    sleep(3);

    close(param.sock);

    return 0;
}

void *tcp_thread_function(void *p)
{
    int len;
    char buffer[PACKMAX];
    struct param_data *param_ptr = (struct param_data*)p;

    while((len = read(param_ptr->sock, buffer, PACKMAX)) > 0)
    {
        struct iphdr *iphdr = (struct iphdr *)buffer;

        if(iphdr->ip_p != IPPROTO_TCP) continue;

        struct tcphdr *tcphdr = (struct tcphdr *)(buffer + (iphdr->ip_hl << 2));

        if((ntohs(tcphdr->th_sport) >= param_ptr->start_port) && (ntohs(tcphdr->th_sport) <= param_ptr->end_port))
        {
            if(((tcphdr->th_flags & TH_SYN) == TH_SYN) && ((tcphdr->th_flags & TH_ACK) == TH_ACK))
                printf("%s : %d\n", inet_ntoa(iphdr->ip_src), ntohs(tcphdr->th_sport));
        }
    }

    return 0;
}

void make_tcp_header(struct tcp_packet *packet, const char *src_ip, uint16_t src_port, const char *dst_ip, uint16_t dst_port, uint32_t seq, uint32_t ack, uint8_t flag)
{
    packet->tcphdr.th_seq = htonl(seq);
    packet->tcphdr.th_ack = htonl(ack);
    packet->tcphdr.th_sport = htons(src_port);
    packet->tcphdr.th_dport = htons(dst_port);
    packet->tcphdr.th_off = 5;
    packet->tcphdr.th_flags = flag;
    packet->tcphdr.th_win = htons(8192);
    packet->tcphdr.th_urp = 0;

    packet->iphdr.ip_ttl = 0;
    packet->iphdr.ip_p = IPPROTO_TCP;
    packet->iphdr.ip_src.s_addr = inet_addr(src_ip);
    packet->iphdr.ip_dst.s_addr = inet_addr(dst_ip);
    packet->iphdr.ip_sum = htons(sizeof(packet->tcphdr));

    packet->tcphdr.th_sum = 0;
    packet->tcphdr.th_sum = cksum((void *)&(packet->iphdr.ip_ttl), PSEUDO_HEADER_LEN + sizeof(packet->tcphdr));
}

void make_ip_header(struct iphdr *iphdr, const char *src_ip, const char *dst_ip, uint16_t datalen)
{
    iphdr->ip_v = 4;
    iphdr->ip_hl = sizeof(struct iphdr) >> 2;
    iphdr->ip_id = 100;
    iphdr->ip_len = htons(sizeof(struct iphdr) + datalen);
    iphdr->ip_off = htons(0);
    iphdr->ip_ttl = 128;
    iphdr->ip_p = IPPROTO_TCP;
    iphdr->ip_src.s_addr = inet_addr(src_ip);
    iphdr->ip_dst.s_addr = inet_addr(dst_ip);
    iphdr->ip_sum = 0;
    iphdr->ip_sum = cksum((void*)iphdr, sizeof(struct iphdr));
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