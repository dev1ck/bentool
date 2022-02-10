#include "protocol.h"

int g_tcp_end_flag=0; //send packet completed

void  strmac_to_buffer(const char *str, uint8_t *mac);
void *tcp_thread_function(void *p);

struct param_data
{
    int sock;
    uint16_t start_port;
    uint16_t end_port;
    struct in_addr src_ip, dst_ip;

};

int half_open_scan(int args, ...)
{
    int on = 1, thread_return;
    uint16_t start_port, end_port, port;
    time_t start_tm = time(NULL), end_tm, play_tm;
    char* if_name;
    char buf[17];
    char src[16], dst[16];
    
    struct sockaddr_in addr;
    struct in_addr my_ip;
    struct timeval tv;
    struct nic_info nic_info;
    struct param_data param;
    struct tcp_packet packet;
    
    pthread_t thread_id;
    va_list ap;
    
    va_start(ap, args);

    if_name = va_arg(ap, char*);
    param.start_port = start_port = (uint16_t)atoi(va_arg(ap, char*));
    param.end_port = end_port = (uint16_t)atoi(va_arg(ap, char*));

    tv.tv_sec = 2;
    tv.tv_usec = 0;

    if((param.sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
    {
        perror("socket ");
        return -1;
    }

    if(setsockopt(param.sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
    {
        perror("setsockopt1 ");
        return -1;
    }
    if((setsockopt(param.sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)) < 0)
    {
        perror("setsocket2 ");
        return -1;
    }

    if(get_info(&nic_info, if_name)<0)
    {
        printf("Interface name error, input -if <interface_name> or don't use -if option\n");
        close(param.sock);
        return -1;
    }

    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    memset(&packet, 0, sizeof(packet));

    printf("\n===== Starting Half Open Scan... =====\n\n");

    switch(args)
    {
        case 3:
            {
                uint32_t start_ip = 0, end_ip = 0, ip = 0, mask = 0;
                va_end(ap);

                mask = htonl(nic_info.maskaddr.s_addr);
                ip = htonl(nic_info.addr.s_addr);

                start_ip = (ip & mask) + 1;
                param.src_ip.s_addr = htonl(start_ip);
                
                end_ip = (ip | ~mask) - 1;
                param.dst_ip.s_addr = htonl(end_ip);

                pthread_create(&thread_id, NULL, tcp_thread_function, &param);
                
                strncpy(src, inet_ntoa(param.src_ip), strlen(inet_ntoa(param.src_ip)));
                strncpy(dst, inet_ntoa(param.dst_ip), strlen(inet_ntoa(param.dst_ip)));

                if(start_port == end_port)
                    printf("Send to TCP SYN flag Packet : %s ~ %s (%d)\n", src, dst, start_port);
                else
                    printf("Send to TCP SYN flag Packet : %s ~ %s (%d ~ %d)\n", src, dst, start_port, end_port);
                printf("\n===== Host List =====\n\n");  

                for(; start_ip <= end_ip; start_ip++)
                {
                    addr.sin_addr.s_addr = htonl(start_ip);
                    //printf("target ip : %s\n", inet_ntoa(addr.sin_addr));

                    for(port = start_port; port <= end_port; port++)
                    {
                        make_tcp_header(&packet.tcphdr, nic_info.addr, rand(), addr.sin_addr, port, rand(), 0, TH_SYN);
                        make_ip_header(&packet.iphdr, nic_info.addr, addr.sin_addr, sizeof(struct tcphdr),rand());

                        if(sendto(param.sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
                        {
                            strncpy(buf, inet_ntoa(addr.sin_addr),strlen(inet_ntoa(addr.sin_addr)));
                            strcat(buf,  " ");
                            perror(buf);
                            break;
                        }
                        if(port == end_port) break;
                    }   
                }
                break;
            }
        case 4:
            {
                char * data = va_arg(ap, char*);
                if(hostname_to_ip(data, &addr.sin_addr) < 0)
                {
                    printf("\"%s\" host not found, check hostname\n", data);
                    close(param.sock);
                    return -1;
                }
                va_end(ap); 
                //printf("%s\n",inet_ntoa(addr.sin_addr));
                param.src_ip.s_addr = addr.sin_addr.s_addr;
                param.dst_ip.s_addr = addr.sin_addr.s_addr;
                pthread_create(&thread_id, NULL, tcp_thread_function, &param);
                
                if(start_port == end_port)
                    printf("Send to TCP SYN flag Packet : %s (%d)\n", inet_ntoa(addr.sin_addr), start_port);
                else
                    printf("Send to TCP SYN flag Packet : %s (%d ~ %d)\n", inet_ntoa(addr.sin_addr), start_port, end_port);

                printf("\n===== Host List =====\n\n");  

                for(port = start_port; port <= end_port; port++)
                {
                    make_tcp_header(&packet.tcphdr, nic_info.addr, rand(), addr.sin_addr, port, rand(), 0, TH_SYN);
                    make_ip_header(&packet.iphdr, nic_info.addr, addr.sin_addr, sizeof(struct tcphdr),rand());

                    if(sendto(param.sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
                    {
                        strncpy(buf, inet_ntoa(addr.sin_addr),strlen(inet_ntoa(addr.sin_addr)));
                        strcat(buf,  " ");
                        perror(buf);
                        break;
                    }
                    if(port == end_port) break;
                }
                break;  
            } 
        case 5:
            {    
                
                param.src_ip.s_addr = inet_addr(va_arg(ap, char*));
                param.dst_ip.s_addr = inet_addr(va_arg(ap, char*));
                va_end(ap);

                
                uint32_t start_ip, end_ip;

                strncpy(src, inet_ntoa(param.src_ip), strlen(inet_ntoa(param.src_ip)));
                strncpy(dst, inet_ntoa(param.dst_ip), strlen(inet_ntoa(param.dst_ip)));
                
                if(start_port == end_port)
                    printf("Send to TCP SYN flag Packet : %s ~ %s (%d)\n", src, dst, start_port);
                else
                    printf("Send to TCP SYN flag Packet : %s ~ %s (%d ~ %d)\n", src, dst, start_port, end_port);

                pthread_create(&thread_id, NULL, tcp_thread_function, &param);
                
                start_ip = ntohl(param.src_ip.s_addr);
                end_ip = ntohl(param.dst_ip.s_addr);
                
                printf("\n===== Host List =====\n\n");  

                for(; start_ip <= end_ip; start_ip++)
                {
                    addr.sin_addr.s_addr = htonl(start_ip);
                    //printf("target ip : %s\n", inet_ntoa(addr.sin_addr));

                    for(port = start_port; port <= end_port; port++)
                    {
                        make_tcp_header(&packet.tcphdr, nic_info.addr, rand(), addr.sin_addr, port, rand(), 0, TH_SYN);
                        make_ip_header(&packet.iphdr, nic_info.addr, addr.sin_addr, sizeof(struct tcphdr),rand());

                        if(sendto(param.sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
                        {
                            
                            strncpy(buf, inet_ntoa(addr.sin_addr),strlen(inet_ntoa(addr.sin_addr)));
                            strcat(buf,  " ");
                            perror(buf);
                            //perror(inet_ntoa(addr.sin_addr));
                            break;
                        }
                        if(port == end_port) break;
                    }  
                }
                break;
            }
    }
    sleep(3);
    g_tcp_end_flag=1;

    pthread_join(thread_id, NULL);
    end_tm = time(NULL);
    play_tm = end_tm - start_tm;

    printf("\nscanned in %lld seconds\n", play_tm);

    close(param.sock);

    return 0;
}


void *tcp_thread_function(void *p)
{
    int len;
    char buffer[PACKET_MAX_LEN];
    struct param_data *param_ptr = (struct param_data*)p;

    do
    {
        if(read(param_ptr->sock, buffer, PACKET_MAX_LEN)<=0) continue;
        struct iphdr *iphdr = (struct iphdr *)buffer;

        if(iphdr->ip_p != IPPROTO_TCP) continue;

        struct tcphdr *tcphdr = (struct tcphdr *)(buffer + (iphdr->ip_hl << 2));
        if((ntohl(iphdr->src_ip.s_addr) >= ntohl(param_ptr->src_ip.s_addr)) && (ntohl(iphdr->src_ip.s_addr) <= ntohl(param_ptr->dst_ip.s_addr)))
        {
            if((ntohs(tcphdr->th_sport) >= param_ptr->start_port) && (ntohs(tcphdr->th_sport) <= param_ptr->end_port))
            {
                if(((tcphdr->th_flags & TH_SYN) == TH_SYN) && ((tcphdr->th_flags & TH_ACK) == TH_ACK))
                    printf("[opened] %s : %d\n", inet_ntoa(iphdr->src_ip), ntohs(tcphdr->th_sport));
            }
        }
    }while(!g_tcp_end_flag);

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