#include "protocol.h"

#define optN 5
enum {i, sA, sP, sT, pA};

int bentool_main(int argc, char **argv)
{
    char c;
    int i_flag;
    char *if_name = IF_NAME;
    int start_arg=0;
    int opt_argc=1;
    int flag[optN]={0,};
    
    while((c=getopt(argc,argv,"i::s::p::"))!=-1)
    {
        switch(c)
        {
            case 'i':
                if(!optarg || optarg[1])
                {   
                    flag[i]=1;
                    for(start_arg=optind ; optind<argc ;optind++)
                        if(argv[optind][0] == 0 || argv[optind][0]=='-')
                            break;
                    if(start_arg!=optind)
                        opt_argc += optind -start_arg;
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
                        flag[sA]=1;
                        for(start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(start_arg!=optind)
                            opt_argc += optind -start_arg;
                        break;
                    case 'P':
                        flag[sP]=1;
                        for(start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(start_arg!=optind)
                            opt_argc += optind -start_arg;
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
                        flag[pA]=1;
                        for(start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(start_arg!=optind)
                            opt_argc += optind -start_arg;
                        break;
                    default:
                        printf("No option\n");
                }
                break;
            case '?':
                return -1;
        }
    }
    int sum = 0;
    for(int i =0; i<optN;i++)
        sum += flag[i];
        
    if(sum==0)
    {
        printf("use option\n");
        return -1;
    }
    else if(sum>1)
    {
        printf("too many option\n");
        return -1;
    }

    if(flag[i])
    {
        switch(opt_argc)
        {
            case 1:
                get_interface_devices(NULL);
                break;
            case 2:
                get_interface_devices(argv[start_arg]);
                break;
            default:
                printf("Too many arguments\n");
        }
    }
    else if(flag[sA])
    {
        switch(opt_argc)
        {
            case 1:
                printf("opt_argc, if_name\n");
                break;
            case 2:
                printf("opt_argc, if_name,argv[start_arg])\n");
                break;
            case 3:
                printf("opt_argc, if_name,if_name,argv[start_arg], argv[start_arg+1])\n");
                break;
            default:
                printf("Too many arguments\n");
        }
    }
    else if(flag[sP])
    {
        switch(opt_argc)
        {
            case 1:
                ping_scan(opt_argc, if_name);
                break;
            case 2:
                ping_scan(opt_argc, if_name,argv[start_arg]);
                break;
            case 3:
                ping_scan(opt_argc, if_name,argv[start_arg], argv[start_arg+1]);
                break;
            default:
                printf("Incorrect use\n");
        }

    }
    else if(flag[sT])
    {   
        switch(opt_argc)
        {
            case 2:
                printf("tcp_scan(opt_argc, if_name,argv[start_arg])\n");
                break;
            case 3:
                printf("tcp_scan(opt_argc, if_name,argv[start_arg], argv[start_arg+1])\n");
                break;
            default:
                printf("Incorrect use\n");
        }

    }
    else if(flag[pA])
    { 
        switch(opt_argc)
        {
            case 3:
                arp_spoof(if_name, argv[start_arg], argv[start_arg+1]);
                break;

            default:
                printf("Incorrect use\n");
        }
    }
    
return 0;
}
