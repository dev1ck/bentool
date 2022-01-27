#include "protocol.h"

/*
int main(void)
{
    struct nic_info nic_info;
    memset(&nic_info, 0, sizeof(nic_info));
    get_info(&nic_info, "eth0");
    printf("%s\n", inet_ntoa(nic_info.in_addr));
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",nic_info.my_mac[0], nic_info.my_mac[1], nic_info.my_mac[2], nic_info.my_mac[3], nic_info.my_mac[4], nic_info.my_mac[5]);
    printf("ifindex : %d\n", nic_info.ifindex);
    printf("nic_info.broadaddr : %s\n", inet_ntoa(nic_info.broadaddr));
    printf("netmask : %s\n", inet_ntoa(nic_info.maskaddr));
    return 0;
}
*/

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
   