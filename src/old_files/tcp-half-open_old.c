#include "protocol.h"

#define PSEUDO_HEADER_LEN 12

void  strmac_to_buffer(const char *str, uint8_t *mac);

//enum {ARGV_CMD, ARGV_GARBAGE_1, ARGV_GARBAGE_2, ARGV_GARBAGE_3, ARGV_GARBAGE_4, ARGV_MY_IP, ARGV_TARGET_IP, ARGV_START_PORT, ARGV_END_PORT};
enum {ARGV_CMD, ARGV_TARGET_IP, ARGV_START_PORT, ARGV_END_PORT};

void *tcp_thread_function(void *p);

struct param_data
{
    int sock;
    uint16_t start_port;
    uint16_t end_port;
};

//int main(int argc, char **argv)
int half_open_scan_old(int argc, char **argv)
{
    pthread_t thread_id;
    int on = 1;
    struct sockaddr_in addr;
    uint16_t start_port, end_port, port;
    struct param_data param;
    struct tcp_packet packet;
    struct nic_info nic_info;

    if(argc<4)
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

    if(get_info(&nic_info, if_name)<0)
    {
        printf("Interface name error, input -if <interface_name> or don't use -if option\n");
        return -1;
    }
    
    param.start_port = start_port = (uint16_t)atoi(argv[ARGV_START_PORT]);
    param.end_port = end_port = (uint16_t)atoi(argv[ARGV_END_PORT]);

    puts("1");
    pthread_create(&thread_id, NULL, tcp_thread_function, &param);

    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[ARGV_TARGET_IP]); 
    memset(&packet, 0x00, sizeof(packet));
    
    

    for(port = start_port; port <= end_port; port += 1)
    {
        make_tcp_header_old(&packet, nic_info.in_addr, rand(), argv[ARGV_TARGET_IP], port, rand(), 0, TH_SYN);
        make_ip_header_old(&(packet.iphdr), nic_info.in_addr, argv[ARGV_TARGET_IP], sizeof(struct tcphdr));
        if(sendto(param.sock, &(packet.iphdr), sizeof(struct iphdr) + sizeof(struct tcphdr), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("sendto ");
            break;
        }
        if(port == end_port) break;
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