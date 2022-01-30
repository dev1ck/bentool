#include "protocol.h"

int relay(uint8_t *dst_mac, char *if_name)
{  
    int sock, len, rlen;
    unsigned char buffer[ETHMAX]={0,};
    struct sockaddr_ll sll, sadr_ll;
    struct nic_info info;
    struct etherhdr* eth_h;
    struct iphdr *ip_header;
    void *iphdr_ptr;
    char * ptr;

    int i = 0;

    if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0)
    {
        perror("socket");
        return -1;
    }
    
    get_info(&info, if_name);

    memset(&sll, 0x00, sizeof(sll));
	sll.sll_family = PF_PACKET;
	sll.sll_ifindex = info.ifindex;
	sll.sll_protocol = htons(ETH_P_ALL);

    if(bind(sock, (struct sockaddr *)&sll, sizeof(sll))<0)
    {
        perror("bind");
        return -1;
    }

    while(1)
    {
        if((rlen = read(sock, buffer, ETHMAX))<0)
            perror("read");

        iphdr_ptr = buffer + sizeof(struct etherhdr);
        eth_h = (struct etherhdr*)buffer;
        ip_header = (struct iphdr*)iphdr_ptr;
        
        if(ip_header->ip_src.s_addr != info.in_addr.s_addr &&
           ip_header->ip_dst.s_addr != info.in_addr.s_addr)
        {
            memcpy(eth_h->ether_dhost,dst_mac,6);
            memcpy(eth_h->ether_shost, info.my_mac, 6);
            
            memset(&sadr_ll, 0, sizeof(sadr_ll));
            sadr_ll.sll_ifindex = info.ifindex;
            sadr_ll.sll_halen = ETH_ALEN; // length of destination mac address
            memcpy(sadr_ll.sll_addr, dst_mac, 6);

            if((len = sendto(sock, buffer, rlen,0,(struct sockaddr*)&sadr_ll,sizeof(sadr_ll)))<0)
                perror("sendto");
        }        
    }
    close(sock);
    return 0;
}

uint8_t* make_arp_request_packet(uint8_t source_mac[6], struct in_addr source_ip, struct in_addr target_ip)
{
    struct etherhdr etherhdr;
    struct arphdr arphdr;
    static uint8_t buffer[ARPMAX];

    arphdr.ar_hrd = htons(0x0001);
    arphdr.ar_pro = htons(0x0800);
    arphdr.ar_hln = 0x06;
    arphdr.ar_pln = 0x04;
    arphdr.ar_op = htons(0x0001);

    memcpy(arphdr.ar_sha, source_mac, 6);
    arphdr.ar_sip = source_ip.s_addr;
    memset(arphdr.ar_tha,0x00,6);
    arphdr.ar_tip = target_ip.s_addr;

    
    memset(etherhdr.ether_dhost, 0xff, 6);
    memcpy(etherhdr.ether_shost, source_mac ,6);
    etherhdr.ether_type = htons(0x0806);

    memcpy(buffer,&etherhdr,sizeof(struct etherhdr));
    memcpy(buffer+sizeof(struct etherhdr), &arphdr, sizeof(struct arphdr));

    return buffer; 
}

uint8_t* make_arp_reply_packet(uint8_t source_mac[6], struct in_addr source_ip, uint8_t target_mac[6], struct in_addr target_ip)
{
    struct etherhdr etherhdr;
    struct arphdr arphdr;
    static uint8_t buffer[ARPMAX];

    arphdr.ar_hrd = htons(0x0001);
    arphdr.ar_pro = htons(0x0800);
    arphdr.ar_hln = 0x06;
    arphdr.ar_pln = 0x04;
    arphdr.ar_op = htons(0x0002);

    memcpy(arphdr.ar_sha, source_mac, 6);
    arphdr.ar_sip = source_ip.s_addr;
    memcpy(arphdr.ar_tha, target_mac ,6);
    arphdr.ar_tip = target_ip.s_addr;

    
    memcpy(etherhdr.ether_dhost, target_mac, 6);
    memcpy(etherhdr.ether_shost, source_mac ,6);
    etherhdr.ether_type = htons(0x0806);

    memcpy(buffer,&etherhdr,sizeof(struct etherhdr));
    memcpy(buffer+14, &arphdr, sizeof(struct arphdr));

    return buffer; 
}

uint16_t cksum(void *data, uint32_t len)
{
	unsigned long sum = 0;
    uint16_t *fd;

    fd = (uint16_t*)data;

	for(; len > 1; len -=2 )
    {
		sum += *fd++;

		if(sum & 0x80000000) 
			sum = (sum & 0xffff) + (sum >> 16);
	}

	if(len == 1) 
    {
		unsigned short i = 0;
		*(uint8_t *)(&i) = *(uint8_t*)fd;
		sum += i;
	}

	while(sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return (sum == 0xffff)?sum:~sum;
}

int get_info(struct nic_info *nic_info, char *if_name)
{
    int sock, i, num=30;
    struct ifconf if_conf;
    struct ifreq *ifr;
    struct sockaddr_in *addr;

    if((sock = socket(PF_INET, SOCK_STREAM,0)) < 0)
    {
        perror("socket ");
        return -1;
    }

    memset(&if_conf, 0, sizeof(if_conf));
    if_conf.ifc_len = sizeof(struct ifreq) * num;
    if_conf.ifc_buf = (__caddr_t)malloc(if_conf.ifc_len);
    memset(if_conf.ifc_buf, 0, sizeof(if_conf.ifc_len));

    if(ioctl(sock, SIOCGIFCONF, (char *)&if_conf) < 0)
    {
        perror("ioctl() - get conf ");
        free(if_conf.ifc_buf);
        close(sock);
        return -1;
    }

    
    for(ifr = if_conf.ifc_req; ifr->ifr_name[0]; ifr++)
    {
        if(!strcmp(ifr->ifr_name, if_name))
        {
            addr = (struct sockaddr_in*)&ifr->ifr_addr;
            nic_info->in_addr.s_addr = addr->sin_addr.s_addr;

            if(ntohl(nic_info->in_addr.s_addr) != INADDR_LOOPBACK)
            {
                if(ioctl(sock, SIOCGIFHWADDR, (char*)ifr) < 0)
                {
                    perror("ioctl() - get mac address ");
                    free(if_conf.ifc_buf);
                    close(sock);
                    return -1;
                }
                memcpy(nic_info->my_mac, ifr->ifr_hwaddr.sa_data, 6);
            }

            if(ioctl(sock, SIOCGIFINDEX, (char*)ifr) < 0)
            {
                perror("ioctl() - get ifindex ");
                free(if_conf.ifc_buf);
                close(sock);
                return -1;
            }
            nic_info->ifindex = ifr->ifr_ifindex;

            if(ioctl(sock, SIOCGIFBRDADDR, (char*)ifr) < 0)
            {
                perror("ioctl() - get broadcast addr ");
                free(if_conf.ifc_buf);
                close(sock);
                return -1;
            }
            addr = (struct sockaddr_in*)&ifr->ifr_broadaddr;
            nic_info->broadaddr.s_addr = addr->sin_addr.s_addr;

            if(ioctl(sock, SIOCGIFNETMASK, (char *)ifr) < 0)
            {
                perror("ioctl() - get ifindex ");
                free(if_conf.ifc_buf);
                close(sock);
                return -1;
            }
            addr = (struct sockaddr_in*)&ifr->ifr_addr;
            nic_info->maskaddr.s_addr = addr->sin_addr.s_addr;
        

            free(if_conf.ifc_buf);
            close(sock);
            return 0;
        }       
    }
    free(if_conf.ifc_buf);
    close(sock);
    return -1;
}