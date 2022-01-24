#include "arp_scan.h"

static unsigned char d_target[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static unsigned char live_ip[6];
static unsigned int live_num;
static unsigned int scannig_time;
/*
int arp_scan(char *v);

int main(int argc,char **argv)
{
    printf("\n%s\n\n",argv[1]);
    arp_scan(argv[1]);
    return 0;
}
*/
//int arp_scan(char *v)
int main(int argc,**argv)
{
    printf("\n\n===== Starting ARP_Scan.. =====\n\n");

    int sock;
    unsigned char *mac_addr= NULL;
    struct sockaddr_ll sll;
    struct sockaddr_in *sin;
    static unsigned char buf[sizeof(struct eth_hdr)+sizeof(struct arp_hdr)];

    
    printf("\n\n===== Creating Socket.. =====\n\n");
    sock=socket(PF_PACKET,SOCK_RAW,0);

    if(sock<0)
    {  
        printf("sock() error! ");
        return -1;
    }

    printf("\n\nSocket is Created!\n\n");

    printf("\n\n===== Loading My Interface.. =====\n\n");

    strcpy(ifr_hw.ifr_name,"eth0");
    strcpy(ifr_ip.ifr_name,"eth0");
    if((ioctl(sock,SIOCGIFHWADDR,&ifr_hw))<0)
    {
        printf("\n\n=====!! Failed Loading MY MAC !!=====\n\n");
        close(sock);
        return -1;
    }
    mac_addr=ifr_hw.ifr_hwaddr.sa_data;

    if((ioctl(sock,SIOCGIFADDR,&ifr_ip))<0)
    {
        printf("\n\n=====!! Failed Loading MY IP !!=====\n\n");
        close(sock);
        return -1;
    }
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr; 
    printf("my ip : %s",inet_ntoa(sin->sin_addr));

    printf("\nsin_addr: %x ", sin->sin_addr);
    

    int i;
    for(i=0;i<6;i++){
        ether.src_addr[i]=(uint8_t )mac_addr[i];
        arp.ar_sha[i]=(uint8_t )mac_addr[i];
    }

    //ether.src_addr = ifr.ifr_hwaddr.sa_data;

    //arp.ar_sha =ifr.ifr_hwaddr.sa_data;
    //ether.src_addr,atoi(ifr.ifr_hwaddr.sa_data));
    
    printf("\n\nMY Interface is Loaded !\n\n");
    
    printf("\n\nmy interface is [%s] \n mac: %02x:%02x:%02x:%02x:%02x:%02x\n\n",ifr_ip.ifr_name,mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);


    printf("\n\n===== Creating Ethernet header.. =====\n\n");

    memset(&sll, 0, sizeof(sll));

    sll.sll_family  = PF_PACKET;
    sll.sll_ifindex=if_nametoindex("eth0");
    sll.sll_protocol = htons(ETH_P_ARP);
    sll.sll_halen   = 6;

    ether.dest_addr[0] = 0xff;
    ether.dest_addr[1] = 0xff;
    ether.dest_addr[2] = 0xff;
    ether.dest_addr[3] = 0xff;
    ether.dest_addr[4] = 0xff;
    ether.dest_addr[5] = 0xff;

    

    ether.frame_type = htons(0x0806);
   
    printf("ether dest:");
    for(i=0;i<6;i++){
        printf(" %0x ",ether.dest_addr[i]);
    }
    printf("\n\nether src:");
    for(i=0;i<6;i++){
        printf(" %0x ",ether.src_addr[i]);
    }

    printf("\n\nEthernet header is Created!\n\n");
    printf("\n\n===== Creating ARP header.. =====\n\n");

    arp.ar_hrd=htons(0x0001); //하드웨어 주소 타입. 이더넷은 1
    arp.ar_pro=htons(0x0800); //프로토콜 타입. ARP는 0x0806
    arp.ar_hln=0x06; //하드웨어 주소 길이 1바이트. MAC주소 길이는 6
    arp.ar_pln=0x04; //프로토콜 주소 길이 1바이트. IP주소 길이는 4
    arp.ar_op=htons(0x0001); //oper코드. 요청or응답 패킷 확인. ARP요청0001 응답0002


    //출발지 IP주소
    arp.ar_sip=sin->sin_addr.s_addr;
    printf("arp_sip:");
        printf(" %0x ",arp.ar_sip);

     //목적지 MAC주소(이걸 채우려고 ARP쓰므로 처음엔 비어있다)
    memset(arp.ar_tha,0,sizeof(arp.ar_tha));

    //목적지 IP주소]
    //printf("argv[] : %s",*v);
    arp.ar_tip[0] = 0xc0;
    

    printf("\n\n===== recoding to buffer.. =====\n\n");
    memcpy(buf, &ether, sizeof(struct eth_hdr));
    memcpy(buf+14, &arp, sizeof(struct arp_hdr));

    printf("\n\n===== sending ARP pacekt ====\n\n");
    printf("\n buffer : %0x \n\n",buf);
    if((sendto(sock, buf,sizeof(struct eth_hdr) + sizeof(struct arp_hdr),0,
        (struct sockaddr *)&sll, sizeof(struct sockaddr_ll)))<0)
        {
            printf("sendto() error!");
            return 0;
        } //보냄

    printf("\n\n===== ARP pacekt is sended! ====\n\n");

    return 0;
}