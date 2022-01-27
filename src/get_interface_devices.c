#include "protocol.h"

#ifndef __linux__
    #include <winsock2.h>
#endif

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

