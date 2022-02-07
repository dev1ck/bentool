#include "protocol.h"

#define IF_NAME "eth0"

#define optN 6
enum {opt_i, opt_sA, opt_sP, opt_sH, opt_pA, opt_aS};

struct opt_status
{
    int flag;
    int s_arg;
    int argc;
};

int opt_count(int * start_arg, int optind, int argc, char **argv)
{
    for(*start_arg = optind; optind<argc ;optind++)
        if(argv[optind][0] == 0 || argv[optind][0]=='-')
            break;
    if(*start_arg!=optind)
        return optind - *start_arg;
    return 0;
}

int bentool_main(int argc, char **argv)
{
    char c;
    int i_flag;
    char *if_name = IF_NAME;
    int start_arg=0;
    int opt_argc=1;
    int flag[optN]={0,};
    
    while((c=getopt(argc,argv,"i::s::p::a::"))!=-1)
    {
        switch(c)
        {
            // i 옵션 check on/off
            case 'i':
                // optarg의 값이 없을 때 / -i 옵션 사용
                if(!optarg)
                {   
                    flag[opt_i]=1;
                    opt_argc += opt_count(&start_arg, optind, argc, argv);
                    break;
                }
                // optarg[0]의 값이 f일 때 / -if 옵션 사용
                // if_name의 변수 값 변경하기 위함
                else if(optarg[0]=='f')
                {
                    // optarg 인자 2개 이상일 때 에러처리 - Ex)ifc
                    if(optarg[1])
                    {
                        printf("Option error\n");
                        return -1;
                    }
                    int if_c;
                    // if옵션 인자 개수 확인
                    for(if_c=optind ; if_c<argc ;if_c++)
                        if(argv[if_c][0] == 0 || argv[if_c][0]=='-')
                            break;
                    // if 인자 개수 0개일 때 에러처리
                    if(if_c == optind)
                    {
                       printf("input interface name\n");
                       return -1;
                    }
                    // if 인자 ./bentool -if opt1 opt2   argc = 4 optind = 2 if_c = 5
                    // if 인자 개수 2개 이상일 때 에러처리
                    else if((if_c - optind) > 1)
                    {
                        printf("Too many arguments\n");
                        return -1;
                    }
                    else
                        if_name = argv[optind];
                }
                else
                    printf("No option\n");
                break;
            // s 옵션 check on/off
            case 's':
                // optarg 의 값이 없거나, 옵션이 2개일 경우 애러처리 
                if(!optarg || optarg[1])
                {   
                    printf("Option error\n");
                    return -1;
                }
                char s = optarg[0];
                switch(s)
                {
                    // 옵션이 sA 일 경우
                    case 'A':
                        flag[opt_sA]=1; // flag[sA] 플래그 on
                        opt_argc += opt_count(&start_arg, optind, argc, argv);
                        break;
                    // 옵션이 sP 일 경우
                    case 'P':
                        flag[opt_sP]=1; // flag[sP] 플래그 on
                        opt_argc += opt_count(&start_arg, optind, argc, argv);
                        break;
                    // 옵션이 sH 일 경우
                    case 'H':
                        flag[opt_sH]=1; // flag[sH] 플래그 on
                        opt_argc += opt_count(&start_arg, optind, argc, argv);
                        break;
                    default:
                        printf("No option\n");
                        return -1;
                }
                break;
            // p 옵션 check on/off
            case 'p':
                if(!optarg)
                {   
                    opt_argc += opt_count(&start_arg, optind, argc, argv);
                    break;
                }
                // optarg 의 값이 없거나, 옵션이 2개일 경우 애러처리 
                else if(optarg[1])
                {   
                    printf("Option error\n");
                    return -1;
                }
                char p = optarg[0];
                switch(p)
                {
                    // 옵션이 pA 일 경우
                    case 'A':
                        flag[opt_pA]=1; // flag[pA] 플래그 on
                        opt_argc += opt_count(&start_arg, optind, argc, argv);
                        break;
                    default:
                        printf("No option\n");
                        return -1;
                }
                break;
            // a 옵션 check on/off
            case 'a':
                // optarg 의 값이 없거나, 옵션이 2개일 경우 애러처리 
                if(!optarg || optarg[1])
                {   
                    printf("Option error\n");
                    return -1;
                }
                char a = optarg[0];
                switch(a)
                {
                    // 옵션이 aS 일 경우
                    case 'S':
                        flag[opt_aS]=1; // flag[aS] 플래그 on
                        opt_argc += opt_count(&start_arg, optind, argc, argv);
                        break;
                    default:
                        printf("No option\n");
                        return -1;
                }
                break;

            // error
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

    if(flag[opt_i])
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
    else if(flag[opt_sA])
    {   
        switch(opt_argc)
        {
            case 1:
                arp_scan(opt_argc, if_name);
                break;
            case 2:
                arp_scan(opt_argc, if_name,argv[start_arg]);
                break;
            case 3:
                arp_scan(opt_argc, if_name,argv[start_arg], argv[start_arg+1]);
                break;
            default:
                printf("Too many arguments\n");
        }
    }
    else if(flag[opt_sP])
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
    else if(flag[opt_sH])
    {   
        switch(opt_argc)
        { 
            case 1:
                half_open_scan(opt_argc, if_name);
                break;
            case 2:
                half_open_scan(opt_argc, if_name, argv[start_arg]);
                break;
            case 3:
                half_open_scan(opt_argc, if_name, argv[start_arg], argv[start_arg+1]);
                break;
            case 4:
                half_open_scan(opt_argc, if_name, argv[start_arg], argv[start_arg+1], argv[start_arg+2]);
                break;
            default:
                printf("Incorrect use\n");
        }
    }
    else if(flag[opt_pA])
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
    else if(flag[opt_aS])
    {
        opt_argc--;
        switch(opt_argc)
        {
            case 1:
                syn_flood(opt_argc, argv[start_arg]);
                break;
            case 2:
                syn_flood(opt_argc, argv[start_arg], argv[start_arg+1]);
                break;

            default:
                printf("Incorrect use\n");
        }
    }
    
    return 0;
}