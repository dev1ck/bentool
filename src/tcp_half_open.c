#include "protocol.h"

void  strmac_to_buffer(const char *str, uint8_t *mac);

void *tcp_thread_function(void *p);

struct param_data
{
    int sock;
    uint16_t start_port;
    uint16_t end_port;
};

int half_open_scan(char* target_ip, char *s_port, char *e_port)
{
    pthread_t thread_id;
    uint16_t start_port, end_port, port;
    int on = 1;
    struct sockaddr_in addr;
    struct in_addr my_ip;
    struct nic_info nic_info;
    struct param_data param;
    struct tcphdr packet;

    if((param.sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
    {
        perror("socket ");
        return 1;
    }

    param.start_port = start_port = (uint16_t)atoi(s_port);
    param.end_port = end_port = (uint16_t)atoi(e_port);

    pthread_create(&thread_id, NULL, tcp_thread_function, &param);

    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(target_ip);

    memset(&packet, 0, sizeof(packet));

    //inet_aton(argv[ARGV_MY_IP] ,&my_ip);
    get_info(&nic_info, "eth0");

    for(port = start_port; port <= end_port; port++)
    {
        make_tcp_header(&packet, nic_info.in_addr, rand(), addr.sin_addr, port, rand(), 0, TH_SYN);

        if(sendto(param.sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
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