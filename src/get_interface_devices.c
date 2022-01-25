#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef __linux__
    #include <winsock2.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif
    #include "pcap.h"

int get_interface_devices(void)
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
                        break;
                }
            }
        }
        
        printf("\n\n");
    }

    pcap_freealldevs(alldevs);
    return 0;
}

