#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/ip_icmp.h>

#define IPv4_BUFFER 20
#define PREFIX_BUFFER 5

void ping_scan(char *IP_addr);