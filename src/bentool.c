#include "protocol.h"

#define BUFSIZE 100
#define not_set_flag 0x0000
#define o_flag 0x0001
#define t_flag 0x0002
#define i_flag 0x0004
#define set_flag 0x1111

int ping_scan(char *input_IP);
void help()
{
    
}

int check_entered_option(int argc, char **argv, unsigned short *opt_flag)
{
	int i;
	// check the argv[] option
    for(i=0; i<argc; i++)
    {	
	    if(!strncmp(argv[i], "-o", 2))
	    {
		    //strncpy(argv_o_backup, argv[i], strlen(argv[i]));
		    //argv_backup = argv[i];
		    *opt_flag += o_flag;
	    }
	    else if(!strncmp(argv[i], "-t", 2))
	    {
		    //strncpy(argv_t_backup, argv[i], strlen(argv[i]));
		    //argv_backup = argv[i];
		    *opt_flag += t_flag;
	    }
		else if(!strncmp(argv[i], "-i", 2))
		{
			*opt_flag += i_flag;
		}
		
    }

    if(*opt_flag == not_set_flag) return 0;
}

int main(int argc, char **argv)
{
    int opt,i=0;
    unsigned short opt_flag= 0;
    int o_opt_flag=0, t_opt_flag=0;
    char o_optarg_arr[BUFSIZE] = {0,};
    char t_optarg_arr[BUFSIZE] = {0,};
//  char *argv_t_backup=(char*)malloc(sizeof(char *));
//  char *argv_o_backup=(char*)malloc(sizeof(char *));

    if(argc < 2)
    {
		printf("Usage: %s -o <option> -t <type>\n", argv[0]);
		printf("ex) %s -o scan -t ping\n",argv[0]);
		exit(1);
    }

    if(check_entered_option(argc, argv, &opt_flag) == -1)
    {
		fprintf(stderr, "there is no option\n");
		exit(1);
    }
	
	// check the argument of options - right argument and when user no input option
    while((opt = getopt(argc, argv, ":o:t:i:h")) != EOF)
    {

        switch(opt)
        {
            case 'o':

				if(!strncmp(optarg, "scan", 4) || !strncmp(optarg, "spoof", 5))
				{
					strncpy(o_optarg_arr, optarg, strlen(optarg));
				}
				else
				{
					printf("%s -o [scan|spoof]\n", argv[0]);
				}
                break;
		
            case 't':
				if(!strncmp(optarg, "arp", 3) || !strncmp(optarg, "ping", 4) || !strncmp(optarg, "tcp", 3))
				{
					strncpy(t_optarg_arr, optarg, strlen(optarg));
				}
				else
				{
					printf("%s -t [arp|ping|tcp]\n", argv[0]);
				}

                break;
	    
	    	case 'h':
				printf("help\n");
				break;

			case 'i':
				get_interface_devices(optarg);
				break;

			case ':':
				if(optopt =='o') printf("test\n");
				else if(optopt == 't') printf("test\n");
				else if(optopt == 'i') get_interface_devices(NULL);
				break;

	    	case '?':
				if(optopt == 'o')	printf("%s -o [scan|spoof] \n", argv[0]);
				else if(optopt == 't')	printf("%s -t [arp|ping|tcp]\n", argv[0]);
				//else if(optopt == 'i') printf("%s\n");
        }
	//argc -= optind;
	//argv += optind;
    }


	// check turn on one of options and when all flags are set 1
    if(opt_flag == set_flag)
    {

    	if(!strncmp(o_optarg_arr, "scan", 4))
    	{
			if(!strncmp(t_optarg_arr, "arp", 3)) printf("arp_scan()\n");
			else if(!strncmp(t_optarg_arr, "ping", 4)) ping_scan(argv[5]);
			else if(!strncmp(t_optarg_arr, "tcp", 3)) tcp_half_scan(argc, argv);	
    	}
    	else if(!strncmp(o_optarg_arr, "spoof", 5))
    	{
	       	if(!strncmp(t_optarg_arr, "arp", 3)) printf("arp_spoofing()\n");
   		}
    }
    else if(opt_flag == o_flag)
    {
	    printf("specify the '-t' option\n");
	    exit(1);
    }
    else if(opt_flag == t_flag)
    {
	    printf("specify the '-o' option\n");
	    exit(1);
    }

//    free(argv_t_backup);
//    free(argv_o_backup);
    return 0;
}
