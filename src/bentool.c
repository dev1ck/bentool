#include "protocol.h"

#define BUFSIZE 100

void help()
{
    
}

void check_entered_option(int argc, char **argv, int *o_opt_flag, int *t_opt_flag)
{
	int i;
	// check the argv[] option
    for(i=0; i<argc; i++)
    {
	    if(!strncmp(argv[i], "-o", 2))
	    {
		    //strncpy(argv_o_backup, argv[i], strlen(argv[i]));
		    //argv_backup = argv[i];
		    *o_opt_flag = 1;
	    }
	    else if(!strncmp(argv[i], "-t", 2))
	    {
		    //strncpy(argv_t_backup, argv[i], strlen(argv[i]));
		    //argv_backup = argv[i];
		    *t_opt_flag = 1;
	    }
    }
}

int main(int argc, char **argv)
{
    int opt,i=0;
    int o_opt_flag=0, t_opt_flag=0;
    char o_optarg_arr[BUFSIZE] = {0,};
    char t_optarg_arr[BUFSIZE] = {0,};
//    char *argv_t_backup=(char*)malloc(sizeof(char *));
//    char *argv_o_backup=(char*)malloc(sizeof(char *));

    if(argc < 2)
    {
		printf("Usage: %s -o <option> -t <type>\n", argv[0]);
		printf("ex) %s -o scan -t ping\n",argv[0]);
		exit(1);
    }

    check_entered_option(argc, argv, &o_opt_flag, &t_opt_flag);

	
	// check the argument of options - right argument and when user no input option
    while((opt = getopt(argc, argv, "o:t:h")) != EOF)
    {

        switch(opt)
        {
            case 'o':
				if(!strncmp(optarg, "scan", 4) || !strncmp(optarg, "spoof", 5))
				{
					strncpy(o_optarg_arr, optarg, strlen(optarg));
					printf("o_optarg_value : %s\n", o_optarg_arr);
				}
				else
				{
					printf("%s -o [scan|spoof]\n", argv[0]);
					exit(1);
				}
				break;
		
            case 't':
				if(!strncmp(optarg, "arp", 3) || !strncmp(optarg, "ping", 4) || !strncmp(optarg, "tcp", 3))
				{
					strncpy(t_optarg_arr, optarg, strlen(optarg));
					printf("t_optarg_value : %s\n", t_optarg_arr);
				}
				else
				{
					printf("%s -t [arp|ping|tcp]\n", argv[0]);
					exit(1);
				}

				break;
	    
			case 'h':
				printf("help\n");
				break;

			/*
			case ':':
				if(optopt == 'o')
				{
					printf("Enter the argument of the '-o' option\n");
				}
				else if(optopt == 't')
				{
					printf("Enter the argument of the '-t' option\n");
				}
				else
				{
					printf("unknown option: -%c\n", optopt);
				}
				break;
			
			*/
			case '?':
				if(optopt == 'o')	printf("%s -o [arp|ping|tcp] \n", argv[0]);
				else if(optopt == 't')	printf("%s -t [arp|ping|tcp]\n", argv[0]);
        }
	//argc -= optind;
	//argv += optind;
    }


	// check turn on one of options and when all flags are set 1
    if(o_opt_flag == 1 && t_opt_flag == 1)
    {

    	if(!strncmp(o_optarg_arr, "scan", 4))
    	{
			if(!strncmp(t_optarg_arr, "arp", 3)) printf("arp_scan()\n");
			else if(!strncmp(t_optarg_arr, "ping", 4)) ping_scan(argv[5]);
			else if(!strncmp(t_optarg_arr, "tcp", 3)) printf("tcp_scan()\n");	
    	}
    	else if(!strncmp(o_optarg_arr, "spoof", 5))
    	{
	       	if(!strncmp(t_optarg_arr, "arp", 3)) printf("arp_spoofing()\n");
   		}
    }
    else if(o_opt_flag == 1 && t_opt_flag == 0)
    {
	    printf("specify the '-t' option\n");
	    exit(1);
    }
    else if(t_opt_flag == 1 && o_opt_flag ==0)
    {
	    printf("specify the '-o' option\n");
	    exit(1);
    }

//    free(argv_t_backup);
//    free(argv_o_backup);
    return 0;
}
