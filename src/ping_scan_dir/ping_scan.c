#include "ping_scan.h"

int ping_scan(char *input_IP) 
{
    char *ptr;
    struct icmp_packet icmp_p;
    int sock, prefix, result;
    u_int32_t start_ip, end_ip, ip, mask;
    pthread_t thread_id;
    struct timeval tv;

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP))<0)
    {
        printf("error socket : %d\n",errno);
        return -1;
    }
    if((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv))<0)
    {
        printf("error socket\n");
        return -1;
    }

    memset(&icmp_p,0,sizeof(icmp_p));
    icmp_p.icmp.icmp_type = 8;
    icmp_p.icmp.icmp_code = 0;
    icmp_p.icmp.icmp_id = 1;
    icmp_p.icmp.icmp_seq = 1;
    icmp_p.icmp.icmp_cksum = cksum((unsigned short*)&icmp_p, sizeof(icmp_p));

    pthread_create(&thread_id, NULL, thread_function, &sock);

    if(!(ptr=strchr(input_IP,'/')))
    {
        ip = ntohl(inet_addr(input_IP));
        if(send_ping(sock, ip, icmp_p)<0)
        {
            printf("error ping\n");
            return -1;
        }    
    }
    else
    {
        strtok(input_IP,"/");

        ip = ntohl(inet_addr(input_IP));
        prefix =  atoi(ptr+1);
        if(prefix>32 || prefix < 0)
        {
            printf("prefix error\n");
            return -1;
        }
        mask = (0xFFFFFFFF << (32 - prefix)) & 0xFFFFFFFF;

        start_ip = (ip&mask)+1;
        end_ip = (ip|~mask)-1;

        for(ip = start_ip; ip<=end_ip; ip++)
        {
            if(send_ping(sock, ip, icmp_p)<0)
                printf("error ping\n");
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

    return 0;
}

void *thread_function(void *p)
{
	char buffer[BUFMAX];
	int sock, len;
    uint32_t addr[255] = {0};
    int index = 0;
    struct sockaddr_in print_src;
    memset(&print_src, 0, sizeof(print_src));

	sock= *((int *)p);

	while((len = read(sock, buffer, BUFMAX))>0)
    {
		struct ip *ip = (struct ip *)buffer;
		int ip_header_len = ip->ip_hl << 2;

		if(ip->ip_p == IPPROTO_ICMP)
        {
			struct icmp *icmp = (struct icmp *)(buffer + ip_header_len);
			if((icmp->icmp_type == 0) && (icmp->icmp_code == 0))
            {
                addr[index]=ntohl(ip->ip_src.s_addr);
                index++;
			}
		}
	}
    quick_sort(addr, 0, index-1);

    for(int i=0; addr[i]!=0 ; i++)
    {
        print_src.sin_addr.s_addr = htonl(addr[i]);
        printf("host up : %s\n",inet_ntoa(print_src.sin_addr));
    }
    printf("\n%d host is up\n",index);
    printf("fin\n");
	return 0;
}

void quick_sort(uint32_t *addr, int start, int end)
{
    if(start >= end) return;

    int pivot = start;
    int i = pivot + 1;
    int j = end; 
    int temp;

    while(i <= j)
    { 
        while(i <= end && addr[i] <= addr[pivot])
        { 
            i++; 
        } 
        while(j > start && addr[j] >= addr[pivot])
        {
            j--; 
        } 
        if(i > j)
        {
            temp = addr[j];
            addr[j] = addr[pivot];
            addr[pivot] = temp; 
        }
        else
        {
            temp = addr[i];
            addr[i] = addr[j];
            addr[j] = temp; 
        } 
    }
    quick_sort(addr, start, j - 1);
    quick_sort(addr, j + 1, end);
}