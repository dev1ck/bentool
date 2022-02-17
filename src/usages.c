#include "usages.h"

void usage()
{   
    printf("\nsyntax : ./bentool <option> [parameter]\n\n");
    printf("example : ./bentool -h\n");
    printf("example : ./bentool -i eth0\n");
    printf("example : ./bentool -sA\n");
    printf("example : ./bentool -sH -if eth1\n");
    printf("example : ./bentool -pA <target ip> <host ip>\n");
}

void usage_help()
{
    printf("\n[Print network interface information]\n");
    printf("./bentool -i [<interface_name>]\n");
    
    printf("\n[ARP Scan]\n");
    printf("./bentool -sA [-if <interface_name>]\n");
    printf("./bentool -sA <target IP> [-if <interface_name>]\n");
    printf("./bentool -sA <start IP> <end IP> [-if <interface_name>]\n");
    
    printf("\n[ICMP Scan]\n");
    printf("./bentool -sP [-if interface_name]\n");
    printf("./bentool -sP <target IP or Domain> [-if <interface_name>]\n");
    printf("./bentool -sP <Network ID>:<prefix> [-if <interface_name>]\n");
    printf("./bentool -sP <start IP> <end IP> [-if <intrface_name>]\n");
    
    printf("\n[TCP half open scan]\n");
    printf("./bentool -sH [-p <port>] [-if <interface_name>]\n");
    printf("./bentool -sH [-p <port> <end_port>] [-if <interface_name>]\n");
    printf("./bentool -sH <target IP> [-p <port> [end_port]] [-if <inerface_name>]\n");
    printf("./bentool -sH <start IP> <end IP> [-p <port> [end_port]] [-if <intrface_name>]\n");

    printf("\n[ARP Spoofing]\n");
    printf("./bentool -pS <target IP> <host IP> [-if <inerface_name>]\n");

    printf("\n[Syn flood attack]\n");
    printf("./bentool -aS <target IP or Domain/port>\n");
    printf("./bentool -aS <target IP> <port> [-l <attack_level>] [-if <interface_name>]\n"); 

    printf("\n[Wi-Fi AP scan]\n");
    printf("./bentool -sW <interface_name> <[-c <Wi-Fi channel>]\n"); 

    printf("\n[Wi-Fi deauth attack]\n");
    printf("./bentool -aW <interface_name> <[-c <Wi-Fi channel>] or [-e <Wi-Fi SSID>]>\n"); 
}

void usage_i()
{
    printf("\n[Print network interface information]\n");
    printf("./bentool -i [<interface_name>]\n\n");
    printf("ex) ./bentool -i eth1\n");
}

void usage_sa()
{

    printf("\n[ARP Scan]\n");
    printf("./bentool -sA [-if <interface_name>]\n");
    printf("./bentool -sA <target IP> [-if <interface_name>]\n");
    printf("./bentool -sA <start IP> <end IP> [-if <interface_name>]\n");
    printf("./bentool -sA -if eth1\n\n");
    printf("ex) ./bentool -sA 192.168.0.1 192.168.0.100 -if eth1\n");
}

void usage_sp()
{

    printf("\n[ICMP Scan]\n");
    printf("./bentool -sP [-if interface_name]\n");
    printf("./bentool -sP <target IP or Domain> [-if <interface_name>]\n");
    printf("./bentool -sP <Network ID>:<prefix> [-if <interface_name>]\n");
    printf("./bentool -sP <start IP> <end IP> [-if <intrface_name>]\n\n");
    printf("ex) ./bentool -sP 172.168.1.1 172.168.255.255 -if eth1\n");
}

void usage_sh()
{
    printf("\n[TCP half open scan]\n");
    printf("./bentool -sH [-p <port>] [-if <interface_name>]\n");
    printf("./bentool -sH [-p <port> <end_port>] [-if <interface_name>]\n");
    printf("./bentool -sH <target IP> [-p <port> [end_port]] [-if <inerface_name>]\n");
    printf("./bentool -sH <start IP> <end IP> [-p <port> [end_port]] [-if <intrface_name>]\n\n");
    printf("ex) ./bentool -sH 192.168.0.1 192.168.0.255 -p 10 100 -if eth1\n");
}

void usage_sw()
{
    printf("\n[Wi-Fi AP scan]\n");
    printf("./bentool -sW <interface_name> [-c <Wi-Fi channel>]\n\n");
    printf("ex) ./bentool -i wlan0\n");
    printf("ex) ./bentool -i wlan0 -c 13\n");
}

void usage_pa()
{
    printf("\n[ARP Spoofing]\n");
    printf("./bentool -pA <target IP> <host IP> [-if <inerface_name>]\n\n");
    printf("ex) ./bentool -pA 192.168.0.20 192.168.0.1\n");
}

void usage_as()
{
    printf("\n[Syn flood attack]\n");
    printf("./bentool -aS <target IP or Domain/port>\n");
    printf("./bentool -aS <target IP> <port> [-l <attack_level>] [-if <interface_name>]\n\n");
    printf("[-l] option is express attack strength,\nyou can decide from 1 to 20. (default level:12)\n\n");
    printf("ex) ./bentool -aS www.bentool.com:80 -l 10\n");
    printf("ex) ./bentool -aS 192.168.0.7 80\n");
}

void usage_aw()
{
    printf("\n[Wi-Fi deauth attack]\n");
    printf("./bentool -aW <interface name> <[-c <Wi-Fi channel>] or [-e <Wi-Fi SSID>]>\n\n"); 
    printf("[-c] option designate Wi-Fi channel,\nyou can decide from 1 to 255.\n\n");
    printf("[-e] option designate Wi-Fi SSID,\n\n");
    printf("ex) ./bentool -aW wlan0 -c 4\n");
    printf("ex) ./bentool -aW wlan0 -e wifi_ssid_5G\n");
    printf("ex) ./bentool -aW wlan0 -c 161 -e wifi_ssid_5G\n");
}
