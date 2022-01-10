#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int sock, str_len,recv_len,recv_cnt;
    struct sockaddr_in sa;
    char message[BUF_SIZE];

    sock=socket(PF_INET, SOCK_STREAM, 0);

    memset(&sa,0,sizeof(sa));
    sa.sin_family = AF_INET;
    inet_aton(argv[1], &sa.sin_addr);
    sa.sin_port=htons(atoi(argv[2]));

    printf("Please wait\n");
    if(connect(sock,(struct sockaddr*)&sa,sizeof(sa))!=-1)
        printf("Success connected\n");

    while(1)
    {
        fputs("Input message(Q to quit) : ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if(!strcmp(message, "q\n") || !strcmp(message,"Q\n"))
        {
            puts("Close socket");
            close(sock);
            break;
        }
        str_len = write(sock,message,strlen(message));
        recv_len=0;

        while(recv_len<str_len)
        {
            recv_cnt = read(sock,&message[recv_len],BUF_SIZE-1);
            recv_len += recv_cnt;
        }
        str_len=
        message[str_len]=0;
        printf("Message from server : %s",message);
    }

    return 0;  
}