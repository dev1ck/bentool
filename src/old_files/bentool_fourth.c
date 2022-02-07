#include "protocol.h"

#define IF_NAME "eth0"
#define START_PORT "0"
#define END_PORT "1023"


struct opt_status
{
    uint16_t flag;
    int argc;
};
// 하나의 서브 옵션을 쓰는 메인 옵션이 많을 때 유용한 함수
// 메인옵션과 서브옵션의 잘못된 매칭을 확인하는 함수
int check_sub_option(int argc, char ** argv, char** main_opts, char* sub_opt) 
{
    int is_main_opt=0, is_sub_opt=0;
    //printf("check argv[1] : %s argv[2] : %s , argv[3] : %s\n", argv[1],argv[2],argv[3]);
    for(int i=0 ; i<(sizeof(main_opts)/sizeof(char*)) ; i++)
    {
        for(int j=1; j < argc; j++)
        {
            if(!strcmp(argv[j], main_opts[i]))
            {
                is_main_opt = 1;
                //printf("check 2 argv[1] : %s argv[2] : %s , argv[3] : %s\n", argv[1],argv[2],argv[3]);
                break;
            }       
        }
        if(is_main_opt) break;
    }
    for(int i=1; i < argc; i++)
    {
        if(!strcmp(argv[i], sub_opt))
        {
            is_sub_opt = 1;
            break;
        }
    }
    // 에러
    if(is_sub_opt && !is_main_opt)
    {
        //main_opt에는 sub_opt를 사용할 수 없습니다.
        //%s is not an option for %s.
        //printf("You can't use %s for %s", sub_opt, main_opt);
        return -1;
    }
    // 정상
    return 0;
}

int opt_count(int* optind, int argc, char **argv)
{
    int start_arg = *optind;
    for(; *optind<argc ;(*optind)++)
        if(argv[*optind][0] == 0 || argv[*optind][0]=='-')
            break;
    return (*optind) - start_arg;
}

int bentool_main(int argc, char **argv)
{
    uint16_t main_flag = 0;
    char c;
    int i_flag;
    char *if_name = IF_NAME;
    char *start_port = START_PORT;
    char *end_port = END_PORT;
    char* ck_sub_opt_p[] = {"-sH"}; // p option
    char* ck_sub_opt_l[] = {"-aS"}; // l option
    char **argv_back = argv;
    int main_argc;

    // if(check_sub_option(argc, argv, ck_sub_opt_p, "-p") <0) return -1;
    // if(check_sub_option(argc, argv, ck_sub_opt_l, "-l") <0) return -1;

    while((c=getopt(argc,argv,"i::s::p::a::"))!=-1)
    {
        switch(c)
        {
            // i 옵션 check on/off
            case 'i':
                // optarg의 값이 없을 때 / -i 옵션 사용
                if(!optarg)
                {   
                    main_flag+=I;
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
                    
                    if_name = argv[optind++];
                }
                else
                    printf("No option\n");
                break;
            // s 옵션 check on/off
            case 's':
                // s뒤에 문자가 없거나, 옵션 문자열이 3글자 이상일 경우 에러처리 
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
                        main_flag+=sA; // flag[sA] 플래그 on
                        break;
                    // 옵션이 sP 일 경우
                    case 'P':
                        main_flag+=sP; // flag[sP] 플래그 on
                        break;
                    // 옵션이 sH 일 경우
                    case 'H':
                        main_flag+=sH; // flag[sH] 플래그 on
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
                    switch(p_c)
                    {
                        case 0:
                            printf("Input Port Number\n");
                            return -1;
                        case 1:
                            start_port = end_port = argv[optind-1];
                            break;
                        case 2:
                            start_port = argv[optind-2];
                            end_port = argv[optind-1];
                            break;
                        default:
                            printf("Too many options as -p");
                            return -1;
                    }
                    break;
                }
                // 옵션문자열 3글자 이상일 경우 애러처리 
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
                        main_flag+=pA; // flag[pA] 플래그 on
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
                        main_flag=aS; // flag[aS] 플래그 on
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

    main_argc = argc-optind;
    switch (main_flag)
    {
        case 0:
            printf("Use Option\n");
            break;
        case I:
            switch(main_argc)
            {
                case 0:
                    get_interface_devices(NULL);
                    break;
                case 1:
                    get_interface_devices(argv[optind]);
                    break;
                default:
                    printf("Too many arguments\n");
            }
            break;
        case sA:
            main_argc++;
            switch(main_argc)
            {
                case 1:
                    arp_scan(main_argc, if_name);
                    break;
                case 2:
                    arp_scan(main_argc, if_name,argv[optind]);
                    break;
                case 3:
                    arp_scan(main_argc, if_name,argv[optind], argv[optind+1]);
                    break;
                default:
                    printf("Too many arguments\n");
            }
            break;        
        case sP:
            main_argc++;
            switch(main_argc)
            {
                case 1:
                    ping_scan(main_argc, if_name);
                    break;
                case 2:
                    ping_scan(main_argc, if_name,argv[optind]);
                    break;
                case 3:
                    ping_scan(main_argc, if_name,argv[optind], argv[optind+1]);
                    break;
                default:
                    printf("Incorrect use\n");
            }
            break;
        case sH:
            main_argc+=2;
            switch(main_argc)
            {
                case 3:
                    half_open_scan(main_argc, if_name, start_port , end_port);
                    break;
                case 4:
                    half_open_scan(main_argc, if_name, start_port , end_port, argv[optind]);
                    break;
                case 5:
                    half_open_scan(main_argc, if_name, start_port , end_port, argv[optind], argv[optind+1]);
                    break;
                default:
                    printf("Incorrect use\n");
            }
            break;
        case pA:
            switch(main_argc)
            {
                case 2:
                    arp_spoof(if_name, argv[optind], argv[optind+1]);
                    break;
                default:
                    printf("Incorrect use\n");
            }
            break;
        case aS:
            switch(main_argc)
            {
                case 1:
                    syn_flood(main_argc, argv[optind]);
                    break;
                case 2:
                    syn_flood(main_argc, argv[optind], argv[optind+1]);
                    break;
                default:
                    printf("Incorrect use\n");
            }
            break;
        default:
            printf("Too many use option\n");
    }
    return 0;
}