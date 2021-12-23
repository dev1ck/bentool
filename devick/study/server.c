#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_message(char *message);

int main(int argc, char *argv[])
{
    int ss, cs; //ss : server socket, cs : client_socket

    char message[] = "Hello World";

    struct sockaddr_in sa, ca; //sa : server_address, ca : client_aaddress

    socklen_t cas; //cas client_address_size

    if(argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    ss = socket(PF_INET, SOCK_STREAM, 0);
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY); //32비트 호스트 바이트 오더를 네트워크 바이트 오더로 변환
    sa.sin_port = htons(atoi(argv[1])); //입력 받은 문자를 정수로 변환 후 네트워크 바이트 순서로 변환

    bind(ss, (struct sockaddr*)&sa, sizeof(sa));

    listen(ss,5);

    cas = sizeof(ca);
    cs = accept(ss,(struct sockaddr*)&ca, &cas);

    write(cs, message, sizeof(message));
    close(cs);
    close(ss);
    return 0;
}

void error_message(char *message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}