#include "ping_scan.h"

int main(int argc, char *argv[]) 
{
    char *ptr;
    struct icmp_packet icmp_p;
    int sock, prefix;
    u_int32_t start_ip, end_ip, ip, mask;
    pthread_t thread_id;
    int result;

    if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP))<0)
    {
        printf("error socket : %d\n",errno);
        exit(1);
    }
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    memset(&icmp_p,0,sizeof(icmp_p));
    icmp_p.icmp.icmp_type = 8;
    icmp_p.icmp.icmp_code = 0;
    icmp_p.icmp.icmp_id = 1;
    icmp_p.icmp.icmp_seq = 1;
    icmp_p.icmp.icmp_cksum = cksum((unsigned short*)&icmp_p, sizeof(icmp_p));

    pthread_create(&thread_id, NULL, thread_function, &sock);

    if(!(ptr=strchr(argv[1],'/')))
    {
        ip = ntohl(inet_addr(argv[1]));
        if(send_ping(sock, ip, icmp_p)<0)
            printf("error ping\n");
        //receive_ping(sock);
    }
    else
    {
        strtok(argv[1],"/");

        ip = ntohl(inet_addr(argv[1]));
        prefix =  atoi(ptr+1);
        if(prefix>32 || prefix < 0)
        {
            exit(1);
        }
        mask = (0xFFFFFFFF << (32 - prefix)) & 0xFFFFFFFF;

        start_ip = (ip&mask)+1;
        end_ip = (ip|~mask)-1;

        for(ip = start_ip; ip<=end_ip; ip++)
        {
            if(send_ping(sock, ip, icmp_p)<0)
                printf("error ping\n");
            //receive_ping(sock);
        }
    }
    
    pthread_join(thread_id, (void *)&result);
    close(sock);

    return 0;
}

uint16_t cksum(uint16_t *data, uint32_t len)
{
	unsigned long sum = 0;

	for(; len > 1; len -=2 ) {
		sum += *data++;

		if(sum & 0x80000000) 
			sum = (sum & 0xffff) + (sum >> 16);
	}

	if(len == 1) {
		unsigned short i = 0;
		*(unsigned char *)(&i) = *(unsigned char *)data;
		sum += i;
	}

	while(sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return (sum == 0xffff)?sum:~sum;
}

int send_ping(int sock, u_int32_t ip, struct icmp_packet icmp_p)
{
    struct sockaddr_in addr;
    char * DDN_ip;

    memset(&addr, 0 ,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(ip);

    if(sendto(sock, &icmp_p, sizeof(icmp_p), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) return -1;
    DDN_ip = inet_ntoa(addr.sin_addr);
    printf("send ping : %s\n", DDN_ip);

    return 1;
}

void *thread_function(void *p)
{
	char buffer[BUFMAX];
	int sock, len;
    //char * DDN[255];
    int index =0;
    struct sockaddr_in *addr;
    in_addr_t addr_len = sizeof(addr);
   	
    //memset(DDN,0,255);

	sock= *((int *)p);

	while((len = recvfrom(sock, buffer, BUFMAX, 0, (struct sockaddr *)addr, &addr_len))>0)
    {
		struct ip *ip = (struct ip *)buffer;
		int ip_header_len = ip->ip_hl << 2;

		if(ip->ip_p == IPPROTO_ICMP)
        {
			struct icmp *icmp = (struct icmp *)(buffer + ip_header_len);
			if((icmp->icmp_type == 0) && (icmp->icmp_code == 0))
            {
                //DDN[index]=inet_ntoa(ip->ip_src);
				printf("from : %s\n", inet_ntoa(ip->ip_src));
                //index++;
			}
		}
	}
    printf("fin\n");
	return 0;
}

// void receive_ping(int sock)
// {
//     char buffer[BUFMAX];
//     int len;
//     struct sockaddr_in *addr;
//     socklen_t addr_len = sizeof(struct sockaddr_in); 

//     while((len = recvfrom(sock, buffer, BUFMAX, MSG_DONTWAIT, (struct sockaddr *)addr, &addr_len))>0)
//     {
//         struct ip *ip = (struct ip *)buffer;
// 		int ip_header_len = ip->ip_hl << 2;
// 		if(ip->ip_p == IPPROTO_ICMP) {
// 			struct icmp *icmp = (struct icmp *)(buffer + ip_header_len);
// 			if((icmp->icmp_type == 0) && (icmp->icmp_code == 0)) {
// 				printf("from : %s\n", inet_ntoa(ip->ip_src));
// 			}
// 		} 
//     }
// }