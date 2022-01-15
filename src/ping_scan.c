#include "ping_scan.h"

int main(int argc, char *argv[]) 
{
    char ipaddr[IPv4_BUFFER], prefix[PREFIX_BUFFER], *ptr;
    struct sockaddr_in dest_addr, src_addr;
    int sock;
    struct icmphdr *icmp;

    if(!(ptr=strchr(argv[1],'/')))
    {
        strcpy(ipaddr, argv[1]);
        if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP))<0)
        {
           exit(1);
        }
        memset(&dest_addr, 0 ,sizeof(dest_addr));
        inet_aton(ipaddr, &dest_addr.sin_addr);
        memset(&icmp,0,sizeof(icmp));
        icmp->type = ICMP_ECHO;
        icmp->checksum = icmp_cksum();
    }
    else
    {
        strtok(argv[1],"/");
        strcpy(ipaddr,argv[1]);
        strcpy(prefix, ptr+1);
        if(atoi(prefix)>32 || atoi(prefix) < 0)
        {
            exit(1);
        }


    }
  
}