#include "ping_scan.h"

int main(int argc, char *argv[]) 
{
    char *ptr;
    struct sockaddr_in src_addr;
    struct icmp_packet icmp_p;
    int sock, prefix;
    u_int32_t start_ip, end_ip, ip, mask; 

    if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP))<0)
    {
        printf("error socket : %d\n",errno);
        exit(1);
    }

    memset(&icmp_p,0,sizeof(icmp_p));
    icmp_p.icmp.type = ICMP_ECHO;
    icmp_p.icmp.checksum = cksum((unsigned short *)&icmp_p, sizeof(struct icmp_packet));
    
    if(!(ptr=strchr(argv[1],'/')))
    {
        //strcpy(ipaddr, argv[1]);
        ip = ntohl(inet_addr(argv[1]));
        send_ping(sock, ip, icmp_p);
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
            send_ping(sock, ip, icmp_p);
        }

    }
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
    char * ddl_ip;

    memset(&addr, 0 ,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(ip);

    if(sendto(sock, &icmp_p, sizeof(icmp_p), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("error ping\n");
        return -1;
        exit(1);
    }
    ddl_ip = inet_ntoa(addr.sin_addr);
    printf("send ping : %s\n", ddl_ip);
    return 1;
}