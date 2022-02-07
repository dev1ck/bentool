//#include "arp_scan.h"
#include "protocol.h"
#define ARP_REPLY 2
#define TIME_SEC 1

int g_stopflag=0; // 쓰레드 read 종료
int g_oipflag=0; // IP가 하나일 때 - one ip flag
int g_revflag=0; // 데이터를 받았다는 flag (timer를 돌리기 위함)
int g_my_ip=0; // 입력한 ip가 자신의 ip일 때

// quick sort 함수에 쓰임. 자신의 아이피와 mac주소를 저장하기 위함 
struct save_addrs
{
    uint32_t ip_addr;
    uint8_t hw_addr[6];  
};

// pthraed join 결과 수신한 결과를 저장하기 위함
struct arp_data
{
    uint8_t target_mac[6];
    uint8_t host_mac[6];
};


void *thread(void*t); // 수신하고 화면에 출력하는 쓰레드
void *timer(); // 타이머
void arp_quick_sort(struct save_addrs *addr, int start, int end); // 화면에 출력하기 전 정렬 하기 위함
int send_arp(int sock, uint8_t my_mac[6], struct in_addr sip, struct in_addr tip, struct sockaddr_ll sll); //패킷 만드는 용도

/*int main(int argc,char **argv)
{

    arp_scan(1,argv[1]);

    return 0;
}
*/

int arp_scan(int argc, ...)
{
    char p_sip[16], p_eip[16];
    char *argv[argc];
    static int sock;
    uint32_t start_argv,end_argv,tmp_ip;

    struct nic_info info;
    struct sockaddr_ll sll;
    struct arp_data *arp_data;
    struct in_addr tip;
    struct in_addr start_ip, end_ip;
    
    pthread_t thread_id ,timer_id;
    
    va_list ap;
    va_start(ap, argc);
    
    clock_t start_time, end_time;
    float work_time;

    for(int i = 0; i < argc; i++)
    {
        argv[i] = va_arg(ap, char *);
    }
    va_end(ap);

    //struct sockaddr recv_ip;
    printf("\n\n===== Starting ARP_Scan.. =====\n");

    start_time=time(NULL);

    if((sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL))) < 0)
    {
        perror("socket ");
        return -1;
    };
    
    if(get_info(&info, argv[0]) < 0)
    {
        printf("\n\n=====!! Failed Loading Interface!!=====\n\n");
        close(sock);
        return -1;
    }
    
    printf("\n\nmy ip : %s", inet_ntoa(info.addr));

    memset(&sll, 0, sizeof(sll));
    sll.sll_ifindex=info.ifindex;
    sll.sll_halen   = ETH_ALEN;
    memset(sll.sll_addr, 0xff, 6);
    
    pthread_create(&thread_id, NULL, thread, &sock);
    pthread_create(&timer_id, NULL, timer, NULL);

    usleep(5); // 수신을 먼저 하지만, 손실 데이터가 존재할 수 있으므로 프로그램에 딜레이 줌

    if(argc==1)
    {
        start_argv = htonl(info.maskaddr.s_addr);
        end_argv = htonl(info.addr.s_addr);

        tmp_ip = (start_argv & end_argv) + 1;
        end_argv = (~start_argv | end_argv) - 1;
        start_argv = tmp_ip;

        start_ip.s_addr = ntohl(start_argv);
        end_ip.s_addr = ntohl(end_argv);
        strncpy(p_sip, inet_ntoa(start_ip),16);
        strncpy(p_eip, inet_ntoa(end_ip),16);
        //strcpy(p_sip, inet_ntoa(start_ip));
        //strcpy(p_eip, inet_ntoa(end_ip));
        
        printf("\n\nSend to ARP Packet : all range / %s ~ %s \n", p_sip, p_eip);
        
        g_revflag=1;
        for(tmp_ip = start_argv; tmp_ip <= end_argv; tmp_ip++)
        {
            tip.s_addr = htonl(tmp_ip);
            send_arp(sock, info.my_mac, info.addr, tip, sll);
            usleep(1);
        }
    }
    else if(argc==2)
    {   
        
        if(inet_aton(argv[1], &tip) == 0)
        {
            printf("ip addr insert error\n");
        }
        if(tip.s_addr==info.addr.s_addr) // 입력한 ip가 내 ip 일 경우
        {
            g_stopflag=1;
            g_my_ip=1;
        }
        printf("\n\nSend To ARP Packet : %s\n", argv[1]);
        
        send_arp(sock, info.my_mac, info.addr, tip, sll);
        
        g_oipflag=1;

    }
    else if(argc==3)
    {
        start_argv=ntohl(inet_addr(argv[1]));
        end_argv=ntohl(inet_addr(argv[2]));
        
        if(start_argv == -1||end_argv == -1)
        {
            printf("\nIP Address error\n");
            return -1;
        }

        if(start_argv > end_argv)
        {
            tmp_ip = start_argv;
            start_argv = end_argv;
            end_argv = tmp_ip;
        }
        if(start_argv == end_argv)
        {
            tip.s_addr = htonl(start_argv);
            if(tip.s_addr == info.addr.s_addr)
            {
                g_stopflag=1;
                g_my_ip=1;
            }
            printf("\n\nSend To ARP Packet : %s\n", argv[1]);

            send_arp(sock,info.my_mac,info.addr,tip,sll);
            g_oipflag=1;

        }
        else
        {
            start_ip.s_addr= htonl(start_argv);
            end_ip.s_addr=htonl(end_argv);
            
            strncpy(p_sip, inet_ntoa(start_ip),16);
            strncpy(p_eip, inet_ntoa(end_ip),16);

            printf("\n\nSend To ARP Packet : %s ~ %s \n", p_sip, p_eip);
            
            g_revflag=1; // 범위 스캔의 경우 타이머가 돌지 않도록 함.
            
            for(tmp_ip = start_argv; tmp_ip <= end_argv; tmp_ip++)
            {
                tip.s_addr = htonl(tmp_ip);
                send_arp(sock,info.my_mac,info.addr,tip,sll);
                usleep(1);
            }
            
        }
        
        
    }
    // else
    // {
    //     printf("\ntarget ip error!\n");
    //     return -1;
    // }

    printf("\n\n===== Live IP =====\n\n");  
    if(g_my_ip==1)
    {
        printf("\n%s is my IP\n", inet_ntoa(tip));
        close(sock);
        exit(1);
    }
    else if(g_oipflag == 0)
    {
        sleep(3);
        g_stopflag=1;
    }

    pthread_join(thread_id, (void **)&arp_data);
    end_time = time(NULL);
    
    printf("\n\nworking time : %.1f\n", (double)(end_time-start_time));
    close(sock);

    return 0;
}

int send_arp(int sock,uint8_t my_mac[6],struct in_addr sip,struct in_addr tip,struct sockaddr_ll sll)
{
    uint8_t *buf;
    int n;
    
    buf = make_arp_request_packet(my_mac,sip,tip);
    
    if((n = sendto(sock, buf, ARP_MAX_LEN, 0, (struct sockaddr *)&sll, sizeof(sll))) < 0)
    {
        perror("sendto ");
        return -1;
    }
    //printf("\nsended!\n");
    return 0;
}


void *thread(void *t)
{
    char rep_buf[ARP_MAX_LEN];
    int sock, index = 0;
    struct etherhdr *etherhdr;
    struct arphdr *arphdr;
    struct in_addr rev_ip;
    struct save_addrs *s_addr;
    
    
    sock = *((int *)t);
    // sock = (int)(*t); ???
  
    s_addr = (struct save_addrs *)malloc(sizeof(struct save_addrs) * 1); // 받는 주소 메모리 공간 동적할당
    
    do 
    { 
        if(read(sock, rep_buf, sizeof(rep_buf)) < 0)
        {
            continue;
        }

        etherhdr = (struct etherhdr *)rep_buf;
        arphdr = (struct arphdr*)(rep_buf + sizeof(struct etherhdr));

        if(etherhdr->ether_type == htons(0x0806))
        {
            if(arphdr->ar_op == htons(0x0002))
            {
                g_revflag=1;
                    
                //printf("arphdr : %02x\n",arphdr->ar_sip); // ??
                s_addr[index].ip_addr = ntohl(arphdr->ar_sip);
                memcpy(&s_addr[index].hw_addr, &arphdr->ar_sha, 6);
                index++;
               
                if((s_addr = (struct save_addrs *)realloc(s_addr, sizeof(struct save_addrs)*(index+1))) == NULL)
                {
                    printf("Memory full error\n");
                    break;
                }
                if(g_oipflag==1)
                {
                    
                    g_stopflag=1;
                }
            }
        }
        
    } while(!g_stopflag);

    s_addr = (struct save_addrs *)realloc(s_addr, sizeof(struct save_addrs)*(index)); // 주소 메모리 공간 재할당

    arp_quick_sort(s_addr, 0, index-1);

    for(int i=0; i < index; i++)
    {
        rev_ip.s_addr = htonl(s_addr[i].ip_addr);
    
        printf("host up : %s [%02x:%02x:%02x:%02x:%02x:%02x]\n",inet_ntoa(rev_ip), s_addr[i].hw_addr[0],s_addr[i].hw_addr[1],s_addr[i].hw_addr[2],s_addr[i].hw_addr[3],s_addr[i].hw_addr[4],s_addr[i].hw_addr[5]);
    }
    printf("\n%d hosts is up\n",index);

    free(s_addr);
    return 0;


}
void *timer()
{
    int time;
    while(1)
    {
        if(g_revflag==0)
        {   
            for(time=0;time<10;time++)
            {
                sleep(1);
            }
            if(g_revflag==0)
            {
                g_stopflag=1;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

}
void arp_quick_sort(struct save_addrs *addr, int start, int end)
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
    arp_quick_sort(addr, start, j - 1);
    arp_quick_sort(addr, j + 1, end);
}


    
    