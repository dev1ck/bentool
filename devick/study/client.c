#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char *argv[])//첫번재 인자와 두번째 인자의 0번 인덱스의 유무 질문
{
    int sock, str_len;
    char message[30];
    struct sockaddr_in sa;

    if(argc!=3) //이부분 질문 
    {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(argv[1]); //서버랑 다른 이 부분 질문 
    sa.sin_port = htons(atoi(argv[2]));

    connect(sock,(struct sockaddr*)&sa, sizeof(sa));

    read(sock, message, sizeof(message));

    printf("Message from server : %s \n", message);
    close(sock);
    return 0;
}