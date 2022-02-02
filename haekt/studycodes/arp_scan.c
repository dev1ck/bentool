//#include "arp_scan.h"
#include "../../src/protocol.h"
#define ARP_REPLY 2
#define TIME_SEC 1

int stopflag =0;

struct save_addrs
{
    uint32_t ip_addr;
    uint8_t hw_addr[6];  
};

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

void *thread(void*t);
void quick_sort(struct save_addrs *addr, int start, int end);
int send_arp(int sock,uint8_t my_mac[6],struct in_addr sip,struct in_addr tip,struct sockaddr_ll sll);

int main(int argc,char **argv)
{
    struct nic_info info;
    int result;
    static int sock;
    struct sockaddr_ll sll,re_sll;
    struct sockaddr_in *sin;
    struct thread_arg thread_arg;
    pthread_t thread_id;
    struct arp_data *arp_data;
    struct in_addr tip;
    uint32_t start_argv,end_argv,tmp_ip;
    struct in_addr start_ip,end_ip;
    char p_sip[16], p_eip[16];
    int reply_len;
    

    //struct sockaddr recv_ip;
    printf("\n\n===== Starting ARP_Scan.. =====\n\n");
    printf("\n\n===== Creating Socket.. =====\n\n");
    sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

    if(sock<0)
    {  
        printf("sock() error! ");
        return -1;
    }

    printf("\n\n===== Loading My Interface.. =====\n\n");

    if(get_info(&info,"eth0")<0)
    {
        printf("\n\n=====!! Failed Loading Interface!!=====\n\n");
        close(sock);
        return -1;
    }
        


    printf("\n\ns ip : %s",inet_ntoa(info.in_addr));

    memset(&sll, 0, sizeof(sll));
    sll.sll_ifindex=info.ifindex;
    sll.sll_halen   = ETH_ALEN;
    memset(sll.sll_addr,0xff,6);
   
    pthread_create(&thread_id,NULL,thread,&sock);

    if(argc==1)
    {
        
    }
    else if(argc==2)
    {   
        inet_aton(argv[1],&tip);

        send_arp(sock,info.my_mac,info.in_addr,tip,sll);

    }
    else if(argc==3)
    {
        start_argv=ntohl(inet_addr(argv[1]));
        end_argv=ntohl(inet_addr(argv[2]));

        if(start_argv==-1||end_argv==-1)
        {
            printf("IP Address error\n");
            return -1;
        }

        if(start_argv>end_argv)
        {
            tmp_ip=start_argv;
            start_argv=end_argv;
            end_argv=tmp_ip;
        }
        if(start_argv==end_argv)
        {
            inet_aton(argv[1],&tip);
            printf("\n\nSend To ARP Packet : %s\n\n",argv[1]);
            send_arp(sock,info.my_mac,info.in_addr,tip,sll);
        }
        else
        {
            start_ip.s_addr= htonl(start_argv);
            end_ip.s_addr=htonl(end_argv);
            strcpy(p_sip,inet_ntoa(start_ip));
            strcpy(p_eip,inet_ntoa(end_ip));
            
            printf("\n\nSend To ARP Packet : %s -> %s \n\n", p_sip,p_eip);

            for(tmp_ip = start_argv; tmp_ip<=end_argv; tmp_ip++)
            {
                tip.s_addr = htonl(tmp_ip);
                send_arp(sock,info.my_mac,info.in_addr,tip,sll);
                usleep(1);
            }
        }
        
        
    }
    else
    {
        printf("target ip error!");
    }

    sleep(0.01);
    printf("\n\n===== Live IP =====\n\n");  
    sleep(3);
    stopflag=1;
    pthread_join(thread_id, (void **)&arp_data);

    

    
    return 0;
}
int send_arp(int sock,uint8_t my_mac[6],struct in_addr sip,struct in_addr tip,struct sockaddr_ll sll)
{
    uint8_t *buf;
    int n;
    buf=make_arp_request_packet(my_mac,sip,tip);
    if((n=sendto(sock, buf,ARPMAX,0,(struct sockaddr *)&sll, sizeof(sll)))<0)
    {
        perror("sendto ");
        return -1;
    }
    //printf("\nsended!\n");
    return 0;
}


void *thread(void *t)
{
    
    struct etherhdr *etherhdr;
    struct arphdr *arphdr;
    char rep_buf[ARPMAX];
    struct in_addr rev_ip;
    int sock;
    sock=*((int *)t);
    struct save_addrs *s_addr;
    int index=0;

    s_addr = (struct save_addrs *)malloc(sizeof(struct save_addrs)* 1);
    
    
    do 
    { 
        if(read(sock, rep_buf, sizeof(rep_buf))<0)
        {
            continue;
        }
        etherhdr = (struct etherhdr *)rep_buf;
        arphdr = (struct arphdr*)(rep_buf+sizeof(struct etherhdr));
        if(etherhdr->ether_type == htons(0x0806))
        {
            if(arphdr->ar_op == htons(0x0002))
            {
                    
                //printf("arphdr : %02x\n",arphdr->ar_sip); // ??
                s_addr[index].ip_addr=ntohl(arphdr->ar_sip);
                memcpy(&s_addr[index].hw_addr, &arphdr->ar_sha, 6);
                index++;

                if((s_addr = (struct save_addrs *)realloc(s_addr, sizeof(struct save_addrs)*(index+1))) == NULL)
                {
                    printf("Memory full error\n");
                    break;
                }
            }
        }
        
    }while(!stopflag);

    s_addr = (struct save_addrs *)realloc(s_addr, sizeof(struct save_addrs)*(index));
    quick_sort(s_addr, 0, index-1);

    for(int i=0; i<index ; i++)
    {
        rev_ip.s_addr = htonl(s_addr[i].ip_addr);
    
        printf("host up : %s [%02x:%02x:%02x:%02x:%02x:%02x]\n",inet_ntoa(rev_ip), s_addr[i].hw_addr[0],s_addr[i].hw_addr[1],s_addr[i].hw_addr[2],s_addr[i].hw_addr[3],s_addr[i].hw_addr[4],s_addr[i].hw_addr[5]);
    }
    printf("\n%d hosts is up\n",index);

    free(s_addr);
    return 0;


}

void quick_sort(struct save_addrs *addr, int start, int end)
{
    if(start >= end) return;

    int pivot = start;
    int i = pivot + 1;
    int j = end; 
    struct save_addrs temp;

    while(i <= j)
    { 
        while(i <= end && addr[i].ip_addr <= addr[pivot].ip_addr) i++;
        while(j > start && addr[j].ip_addr >= addr[pivot].ip_addr) j--;

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


    
    