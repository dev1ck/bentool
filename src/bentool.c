#include "protocol.h"

#define optionN 6
enum {sA, sP, sT, pA, aS, i};

int main(int argc, char **argv)
{
    char c;
    struct opt opt[optionN];
    int i_flag;
    char *if_name = IF_NAME;
    
    for(int i =0;i<optionN;i++)
    {
        memset(&opt, 0, sizeof(opt));
    }
    while((c=getopt(argc,argv,"s::p::a::i::"))!=-1)
    {
        switch(c)
        {
            case 's':
                if(!optarg || optarg[1])
                {   
                    printf("error\n");
                    exit(1);
                }
                char s = optarg[0];
                switch(s)
                {
                    case 'A':
                        opt[sA].opt_flag=1;
                        for(opt[sA].start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(opt[sA].start_arg!=optind)
                        {
                            opt[sA].argc= optind - opt[sA].start_arg;
                        }
                        break;
                    case 'P':
                        opt[sP].opt_flag=1;
                        for(opt[sP].start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(opt[sP].start_arg!=optind)
                        {
                            opt[sP].argc= optind - opt[sP].start_arg;
                        }
                        break;
                }
                break;
            case 'p':
                if(!optarg || optarg[1])
                {   
                    printf("error\n");
                    exit(1);
                }
                char p = optarg[0];
                switch(p)
                {
                    case 'A':
                        opt[pA].opt_flag=1;
                        for(opt[pA].start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(opt[pA].start_arg!=optind)
                        {
                            opt[pA].argc= optind - opt[pA].start_arg;
                        }
                        break;
                    default:
                        printf("No option\n");
                }
                break;
            case 'i':
                if(!optarg || optarg[1])
                {   
                    opt[i].opt_flag=1;
                        for(opt[i].start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(opt[i].start_arg!=optind)
                        {
                            opt[i].argc= optind - opt[i].start_arg;
                        }
                        break;
                }
                else if(optarg[0]=='f')
                {
                    int if_c;
                    for(if_c=optind ; if_c<argc ;if_c++)
                        if(argv[if_c][0] == 0 || argv[if_c][0]=='-')
                            break;
                    if(if_c == optind)
                    {
                       printf("input interface name\n");
                       return -1;
                    }
                    else if((if_c - optind) > 1)
                    {
                        printf("Too many interface name option\n");
                        return -1;
                    }
                    else
                        if_name = argv[optind];
                }
                break;
            case '?':
                printf("?optopt : %c\n",optopt);
                break;
        }
    }
    int sum = 0;
    for(int i =0; i<optionN;i++)
        sum += opt[i].opt_flag;
        
    if(sum==0)
    {
        printf("use option\n");
        exit(1);
    }
    else if(sum>1)
    {
        printf("too many option\n");
        exit(1);
    }

    if(opt[sA].opt_flag)
    {
        opt[sA].argc++;
        switch(opt[sA].argc)
        {
            case 1:
                printf("arp_scan(opt[sA].argc, if_name)\n");
                break;
            case 2:
                printf("arp_scan(opt[sA].argc, if_name,argv[opt[sA].start_arg])\n");
                break;

            case 3:
                printf("arp_scan(opt[sA].argc, if_name,argv[opt[sA].start_arg], argv[opt[sA].start_arg+1])\n");
                break;

            default:
                printf("Too many arguments\n");
        }
    }
    else if(opt[sP].opt_flag)
    {
        opt[sP].argc++;
        switch(opt[sP].argc)
        {
            case 1:
                ping_scan(opt[sP].argc, if_name);
                break;
            case 2:
                ping_scan(opt[sP].argc, if_name,argv[opt[sP].start_arg]);
                break;

            case 3:
                ping_scan(opt[sP].argc, if_name,argv[opt[sP].start_arg], argv[opt[sP].start_arg+1]);
                break;

            default:
                printf("Too many arguments\n");
        }

    }
    else if(opt[sT].opt_flag)
    {   
        opt[sT].argc++;

    }
    else if(opt[pA].opt_flag)
    { 
        opt[pA].argc++;
    }
    else if(opt[i].opt_flag)
    {
        switch(opt[i].argc)
        {
            case 0:
                get_interface_devices(NULL);
                break;
            case 1:
                get_interface_devices(argv[opt[i].start_arg]);
                break;
            default:
                printf("Too many arguments\n");
        }
    }
return 0;
}
