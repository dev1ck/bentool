#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "tcplib.h"

#define BUFMAX 4096

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

        if(sendto(param.sock, &(packet.iphdr), sizeof(struct ip) + sizeof(struct tcphdr), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
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
    char buffer[BUFMAX];
    struct param_data *param_ptr = (struct param_data*)p;

    while((len = read(param_ptr->sock, buffer, BUFMAX)) > 0)
    {
        struct ip *iphdr = (struct ip *)buffer;

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
