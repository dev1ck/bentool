#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024

int main(int argc, char * argv[])
{
    int ss, cs,str_len;
    struct sockaddr_in sa, ca;
    socklen_t cas;
    char message[BUF_SIZE];

    ss=socket(PF_INET, SOCK_STREAM, 0);

    memset(&sa,0,sizeof(sa));

    sa.sin_family=AF_INET;
    sa.sin_addr.s_addr=htonl(INADDR_ANY);
    sa.sin_port=htons(atoi(argv[1]));

    bind(ss, (struct sockaddr *)&sa, sizeof(sa));
    listen(ss, 5);
    cas = sizeof(ca);

    for(int i =0 ; i<5 ; i++)
    {
        cs = accept(ss, (struct sockaddr *)&ca, &cas);
        while((str_len=read(cs, message, BUF_SIZE))!=0)
            printf("%s\n",message);
            write(cs, message, str_len);
        close(cs);
    }
    close(ss);

    return 0;
}