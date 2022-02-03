#include "protocol.h"

int g_end_send_fleg=0;

void *thread_function(void *p);
void quick_sort(uint32_t * addr, int start, int end);


int ping_scan(int argc, ...)
{
    char *ptr;
    char *if_name;
    char *input_data;
    struct icmp_packet icmp_p;
    struct sockaddr_in addr;
    int sock, prefix, result;
    u_int32_t start_ip, end_ip, ip, mask;
    pthread_t thread_id;
    struct timeval tv;
    struct in_addr save_start_ip, save_end_ip;
    char p_sip[16], p_eip[16];
    time_t start_tm, end_tm, play_tm;
    struct tm* ptimeinfo;
    struct nic_info info;
    va_list ap;

    va_start(ap, argc);

    if_name = va_arg(ap,char *);
    
    start_ip=end_ip=ip=mask=prefix=0;

    start_tm = time(NULL);
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP))<0)
    {
        perror("socket");
        return -1;
    }
    if((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv))<0)
    {
        perror("setsocket");
        return -1;
    }

    memset(&addr, 0 ,sizeof(addr));
    addr.sin_family = AF_INET;

    memset(&icmp_p,0,sizeof(icmp_p));
    icmp_p.icmp.icmp_type = 8;
    icmp_p.icmp.icmp_code = 0;
    icmp_p.icmp.icmp_id = 1;
    icmp_p.icmp.icmp_seq = 1;
    icmp_p.icmp.icmp_cksum = cksum((void *)&icmp_p, sizeof(struct icmphdr));

    pthread_create(&thread_id, NULL, thread_function, &sock);

    switch(argc)
    {
        case 1:
            if(get_info(&info, if_name)<0)
            {
                printf("Interface name error, input -if <interface_name> or don't use -if option\n");
                return -1;
            }
            
            mask = ntohl(info.maskaddr.s_addr);
            ip = ntohl(info.in_addr.s_addr);

            start_ip = (ip&mask)+1;
            end_ip = (ip|~mask)-1;
            va_end(ap);
            break;
            
        case 2:
            input_data = va_arg(ap,char *);
            if(!(ptr=strchr(input_data,'/')))
            {
                ip = ntohl(inet_addr(input_data));
                if(ip == -1)
                {
                    printf("IP Address error\n");
                    return -1;
                }
            }
            else
            {
                strtok(input_data,"/");

                ip = ntohl(inet_addr(input_data));
                if(ip == -1)
                {
                    printf("IP Address error\n");
                    return -1;
                }

                prefix =  atoi(ptr+1);
                if(prefix>32 || prefix < 0)
                {
                    printf("Prefix error\n");
                    return -1;
                }
                mask = (0xFFFFFFFF << (32 - prefix)) & 0xFFFFFFFF;

                start_ip = (ip&mask)+1;
                end_ip = (ip|~mask)-1;
            }
            va_end(ap);
            break;

        case 3:
            start_ip = ntohl(inet_addr(va_arg(ap,char *)));
            end_ip = ntohl(inet_addr(va_arg(ap,char *)));
            va_end(ap);

            if(start_ip == -1 || end_ip == -1)
            {
                printf("IP Address error\n");
                return -1;
            }
            else if(start_ip>end_ip)
            {
                ip = start_ip;
                start_ip = end_ip;
                end_ip = ip;
            }
            else if(start_ip == end_ip)
            {
                ip = start_ip;
                start_ip = 0;
            }
            break;
    }
    
    if(!start_ip)
    {  
        printf("Send to ICMP Packet : %s\n\n",input_data);
        addr.sin_addr.s_addr = htonl(ip);
       
        if(sendto(sock, &icmp_p, sizeof(icmp_p), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("sendto");
            return -1;
        }    
    }
    else
    {
        save_start_ip.s_addr = htonl(start_ip);
        save_end_ip.s_addr = htonl(end_ip);
        
        strcpy(p_sip,inet_ntoa(save_start_ip));
        strcpy(p_eip,inet_ntoa(save_end_ip));
        printf("Send To ICMP Packet : %s ~ %s\n\n",p_sip, p_eip);
        for(ip = start_ip; ip<=end_ip; ip++)
        {
            addr.sin_addr.s_addr = htonl(ip);
            sendto(sock, &icmp_p, sizeof(icmp_p), 0, (struct sockaddr *)&addr, sizeof(addr));
            usleep(1);
        }

    }
   
    sleep(1);
    g_end_send_fleg=1;
    pthread_join(thread_id, (void *)&result);
    close(sock);

    end_tm = time(NULL);
    play_tm = end_tm - start_tm;

    printf("scanned in %lld seconds\n",play_tm);
    
    return 0;
}

void *thread_function(void *p)
{
	char buffer[PACKMAX];
	int sock;
    uint32_t * addr;
    int index = 0;
    struct sockaddr_in temp_addr;
    socklen_t len = sizeof (struct sockaddr_in);
    char hostname[NI_MAXHOST];

	sock= *((int *)p);

    addr = (uint32_t *)malloc(sizeof(uint32_t) * 1);

	do
    {
        if(read(sock, buffer, PACKMAX)<=0)
        {
            continue;
        }
		struct iphdr *ip = (struct iphdr *)buffer;
		int ip_header_len = ip->ip_hl << 2;

		if(ip->ip_p == IPPROTO_ICMP)
        {
			struct icmphdr *icmp = (struct icmphdr *)(buffer + ip_header_len);
			if((icmp->icmp_type == 0) && (icmp->icmp_code == 0))
            {
                addr[index]=ntohl(ip->ip_src.s_addr);
                index++;
                if((addr = (uint32_t *)realloc(addr, sizeof(uint32_t)*(index+1))) == NULL)
                {
                    printf("Memory full error\n");
                    break;
                }
			}
        }
	}while((!g_end_send_fleg));
    addr = (uint32_t *)realloc(addr, sizeof(uint32_t)*index);
    quick_sort(addr, 0, index-1);

    memset(&temp_addr, 0, sizeof(struct sockaddr_in));
    temp_addr.sin_family = AF_INET;

    for(int i=0; i<index ; i++)
    {
        temp_addr.sin_addr.s_addr = htonl(addr[i]);

        if(getnameinfo((struct sockaddr *)&temp_addr,len, hostname, sizeof(hostname), NULL, 0, NI_NAMEREQD))
            printf("host up : %s\n",inet_ntoa(temp_addr.sin_addr));
        else
            printf("host up : %s (%s)\n",inet_ntoa(temp_addr.sin_addr), hostname);
    }
    printf("\n%d hosts is up\n",index);

    free(addr);
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
        while(i <= end && addr[i] <= addr[pivot]) i++;
        while(j > start && addr[j] >= addr[pivot]) j--;

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