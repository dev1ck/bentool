#include "protocol.h"

int get_interface_devices(char *arg)
{
    pcap_if_t *alldevs, *alldevs_device;
    int i = 0;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_addr *if_addr;

    
    if(pcap_findalldevs(&alldevs, errbuf) < 0)
    {
        printf("pcap_findalldevs error\n");
        return 1;
    }

    if(!alldevs)
    {
        printf("%s\n", errbuf);
    }

    if(arg == NULL)
    {
        for(alldevs_device=alldevs; alldevs_device; alldevs_device=alldevs_device->next)
        {
            printf("%d. interface name : %s", ++i, alldevs_device->name);

            if(alldevs_device->description)
                printf(" (%s)", alldevs_device->description);
        
            if(alldevs_device->addresses)
            {
                for(if_addr=alldevs_device->addresses; if_addr; if_addr=if_addr->next)
                { 
                    struct sockaddr_in *in_addr = (struct sockaddr_in *)if_addr->addr;
                    struct sockaddr_in *in_addr_netmask = (struct sockaddr_in*)if_addr->netmask;
                    struct sockaddr_in *in_addr_broadaddr = (struct sockaddr_in*)if_addr->broadaddr; 

                    switch(if_addr->addr->sa_family)
                    {
                        case AF_INET:
                            printf("\nIP : %s", inet_ntoa(in_addr->sin_addr));
                            printf("\nnetmask : %s", inet_ntoa(in_addr_netmask->sin_addr));
                            if(alldevs_device->flags != 55)
                            printf("\nbroadcast address : %s", inet_ntoa(in_addr_broadaddr->sin_addr));
                            //return in_addr->sin_addr;
                            break;
                    }
                }
            }
            printf("\n\n");
        }
    }
    else if(arg)
    {
        for(alldevs_device=alldevs; alldevs_device; alldevs_device=alldevs_device->next)
        {
            if(!strncmp(alldevs_device->name, arg, strlen(arg))) break;
        }
        
        printf("interface name : %s\n", alldevs_device->name);
        if(alldevs_device->description)
            printf(" (%s)", alldevs_device->description);
        
        if(alldevs_device->addresses)
        {
            for(if_addr=alldevs_device->addresses; if_addr; if_addr=if_addr->next)
            { 
                struct sockaddr_in *in_addr = (struct sockaddr_in *)if_addr->addr;
                struct sockaddr_in *in_addr_netmask = (struct sockaddr_in*)if_addr->netmask;
                struct sockaddr_in *in_addr_broadaddr = (struct sockaddr_in*)if_addr->broadaddr; 
                switch(if_addr->addr->sa_family)
                {
                    case AF_INET:
                        printf("\nIP : %s", inet_ntoa(in_addr->sin_addr));
                        printf("\nnetmask : %s", inet_ntoa(in_addr_netmask->sin_addr));
                        if(alldevs_device->flags != 55)
                        printf("\nbroadcast address : %s", inet_ntoa(in_addr_broadaddr->sin_addr));
                        break;
                }
            }
        }
    }
    pcap_freealldevs(alldevs);
    return 0;
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
            nic_info->addr.s_addr = addr->sin_addr.s_addr;

            if(ntohl(nic_info->addr.s_addr) != INADDR_LOOPBACK)
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

int relay(uint8_t *dst_mac, char *if_name)
{  
    int sock, len, rlen;
    unsigned char buffer[ETH_MAX_LEN]={0,};
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
        if((rlen = read(sock, buffer, ETH_MAX_LEN))<0)
            perror("read");

        iphdr_ptr = buffer + sizeof(struct etherhdr);
        eth_h = (struct etherhdr*)buffer;
        ip_header = (struct iphdr*)iphdr_ptr;
        
        if(ip_header->src_ip.s_addr != info.addr.s_addr &&
           ip_header->dst_ip.s_addr != info.addr.s_addr)
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
    static uint8_t buffer[ARP_MAX_LEN];

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
    static uint8_t buffer[ARP_MAX_LEN];

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

void make_tcp_header(struct tcphdr *packet, struct in_addr src_ip, uint16_t src_port, struct in_addr dst_ip, uint16_t dst_port, uint32_t seq, uint32_t ack, uint8_t flag)
{
    struct tcp_cksum_hdr cksum_hdr;
    memset(&cksum_hdr, 0, sizeof(cksum_hdr));

    cksum_hdr.pseudohdr.src_ip = src_ip;
    cksum_hdr.pseudohdr.dst_ip = dst_ip;
    cksum_hdr.pseudohdr.protocol_type = IPPROTO_TCP;
    cksum_hdr.pseudohdr.tcp_total_length = htons(sizeof(struct tcphdr));

    packet->th_sport = htons(src_port);
    packet->th_dport = htons(dst_port);
    packet->th_seq = htonl(seq);
    packet->th_ack = htonl(ack);
    packet->th_off = 5;
    packet->th_flags = flag;
    packet->th_win = htons(8192);
 
    packet->th_sum = 0;
    cksum_hdr.tcphdr = *packet; 

    packet->th_sum = cksum((void *)&cksum_hdr, sizeof(cksum_hdr));
}

void make_ip_header(struct iphdr *iphdr, struct in_addr src_ip, struct in_addr dst_ip, uint16_t datalen)
{
    iphdr->ip_v = 4;
    iphdr->ip_hl = sizeof(struct iphdr) >> 2;
    iphdr->ip_id = 100;
    iphdr->ip_len = htons(sizeof(struct iphdr) + datalen);
    iphdr->ip_off = htons(0);
    iphdr->ip_ttl = 128;
    iphdr->ip_p = IPPROTO_TCP;
    iphdr->src_ip = src_ip;
    iphdr->dst_ip = dst_ip;
    iphdr->ip_sum = 0;
    iphdr->ip_sum = cksum((void*)iphdr, sizeof(struct iphdr));
}

void make_tcp_header_old(struct tcp_packet *packet, const char *src_ip, uint16_t src_port, const char *dst_ip, uint16_t dst_port, uint32_t seq, uint32_t ack, uint8_t flag)
{
    packet->tcphdr.th_seq = htonl(seq);
    packet->tcphdr.th_ack = htonl(ack);
    packet->tcphdr.th_sport = htons(src_port);
    packet->tcphdr.th_dport = htons(dst_port);
    packet->tcphdr.th_off = 5;
    packet->tcphdr.th_flags = flag;
    packet->tcphdr.th_win = htons(8192);
    packet->tcphdr.th_urp = 0;

    packet->iphdr.ip_ttl = 0;
    packet->iphdr.ip_p = IPPROTO_TCP;
    packet->iphdr.src_ip.s_addr = inet_addr(src_ip);
    packet->iphdr.dst_ip.s_addr = inet_addr(dst_ip);
    packet->iphdr.ip_sum = htons(sizeof(packet->tcphdr));

    packet->tcphdr.th_sum = 0;
    packet->tcphdr.th_sum = cksum((void *)&(packet->iphdr.ip_ttl), PSEUDO_HEADER_LEN + sizeof(packet->tcphdr));


}

void make_ip_header_old(struct iphdr *iphdr, const char *src_ip, const char *dst_ip, uint16_t datalen)
{
    iphdr->ip_v = 4;
    iphdr->ip_hl = sizeof(struct iphdr) >> 2;
    iphdr->ip_id = 100;
    iphdr->ip_len = htons(sizeof(struct iphdr) + datalen);
    iphdr->ip_off = htons(0);
    iphdr->ip_ttl = 128;
    iphdr->ip_p = IPPROTO_TCP;
    iphdr->src_ip.s_addr = inet_addr(src_ip);
    iphdr->dst_ip.s_addr = inet_addr(dst_ip);
    iphdr->ip_sum = 0;
    iphdr->ip_sum = cksum((void*)iphdr, sizeof(struct iphdr));
}

int hostname_to_ip(char * hostname , struct in_addr *ip)
{
    struct hostent *he;
    if((he=gethostbyname(hostname))==NULL)
        return -1;
    memcpy(ip, he->h_addr_list[0], 4);
    return 0;
}
