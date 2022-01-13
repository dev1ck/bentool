#include <stdio.h>
#include <unistd.h>
#include <string.h>
char *optarg_value;

void help()
{
    
}

int main(int argc, char **argv)
{
    int opt;

    while((opt = getopt(argc, argv, "o:t::")) != -1)
    {
        switch(opt)
        {
            case 'o':
                if(optarg == NULL)
                {
                    printf("input argument\n");
                    printf("optopt : %s\n", optopt);
                }  
                else
                {
                    printf("optarg : %s\n", optarg);
                    optarg_value = optarg;
                }
                
                break;
            case 't':
                printf("%s\n", optarg_value);
                
                if(!strcmp(optarg_value, "scan"))
                {
                    printf("t option first if() success!\n");
                    if(!strcmp(optarg, "arp"))
                    {
                        printf("t option second if() success!\n");
                        printf("arp_scan()\n");
                        //arp_scan();
                    } 
                    else if(!strcmp(optarg, "ping"))
                    {  
                        printf("ping_scan()\n");
                        //ping_scan();
                    }
                    else if(!strcmp(optarg,"tcp"))
                    {
                        printf("tcp_scan()\n");
                        //tcp_scan();
                    }
                }
                else if(!strcmp(optarg_value, "spoof"))
                {
                    if(!strcmp(optarg, "arp"))
                    {
                        printf("arp_spoofing()\n");
                    }

                }
                break;
            case ':':
                printf(": error\n");
                break;
            case '?':
                break;

        }
    }
    
    return 0;
}