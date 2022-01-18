#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 100

void help()
{
    
}

int main(int argc, char **argv)
{
    int opt,i=0;
    int o_option_flag=0, t_option_flag=0;
    char o_optarg_arr[BUFSIZE] = {0,};
    char t_optarg_arr[BUFSIZE] = {0,};
    char *argv_t_backup=(char*)malloc(sizeof(char *));
    char *argv_o_backup=(char*)malloc(sizeof(char *));

    if(argc < 2)
    {
	printf("Usage: %s -o <option> -t <type>\n", argv[0]);
	printf("ex) %s -o scan -t ping\n",argv[0]);
	exit(1);
    }

    for(i=0; i<argc; i++)
    {
	    if(!strcmp(argv[i], "-t"))
	    {
		    strcpy(argv_t_backup, argv[i]);
		    //argv_backup = argv[i];
		    t_option_flag = 1;
	    }
	    else if(!strcmp(argv[i], "-o"))
	    {
		    strcpy(argv_o_backup, argv[i]);
		    //argv_backup = argv[i];
		    o_option_flag = 1;
	    }
    }

    while((opt = getopt(argc, argv, ":o:t:h")) != EOF)
    {

        switch(opt)
        {
            case 'o':
		    if(!strcmp(optarg, "scan") || !strcmp(optarg, "spoof"))
		    {
			strcpy(o_optarg_arr, optarg);
			printf("o_optarg_value : %s\n", o_optarg_arr);
		    }
		    else
		    {
			printf("%s -o [scan|spoof]\n", argv[0]);
			return 1;
		    }
                break;
		
            case 't':
		    if(!strcmp(optarg, "arp") || !strcmp(optarg, "ping") || !strcmp(optarg, "tcp"))
		    {
			strcpy(t_optarg_arr, optarg);
			printf("t_optarg_value : %s\n", t_optarg_arr);
		    }
		    else
		    {
			printf("%s -t [arp|ping|tcp]\n", argv[0]);
			return 1;
		    }

                break;
	    
	    case 'h':
		printf("help\n");
		break;
            
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
        }
	//argc -= optind;
	//argv += optind;
    }


    if(o_option_flag == 1 && t_option_flag == 1)
    {

    	if(!strcmp(o_optarg_arr, "scan"))
    	{
	    if(!strcmp(t_optarg_arr, "arp")) printf("arp_scan()\n");
	    else if(!strcmp(t_optarg_arr, "ping")) printf("ping_scan()\n");
	    else if(!strcmp(t_optarg_arr, "tcp")) printf("tcp_scan()\n");	
    	}
    	else if(!strcmp(o_optarg_arr, "spoof"))
    	{
	       	if(!strcmp(t_optarg_arr, "arp")) printf("arp_spoofing()\n");
   	}
    }
    else if(o_option_flag == 1 && t_option_flag == 0)
    {
	    printf("specify the '-t' option\n");
	    return 1;
    }
    else if(t_option_flag == 1 && o_option_flag ==0)
    {
	    printf("specify the '-o' option\n");
	    return 1;
    }

    free(argv_t_backup);
    free(argv_o_backup);
    return 0;
}
