#include <stdio.h>
#include <unistd.h>


int main(int argc, char **argv)
{
    char c;
    int i=0;
    for(; i < argc; i++)
    {
        printf("argv[%d] value : %s\n", i, argv[i]);
    }
    printf("\n\n");

    while((c = getopt(argc, argv, "a::b::c::d::")) != -1)
    {
        switch(c)
        {
            case 'a':
                {
                    printf("optarg : %s\n", optarg);
                    //printf("optarg 2 : %s\n", ++optarg);
                    printf("a optind : %d\n", optind);
                    printf("a optind 2 : %s\n", argv[optind++]);
                    //printf("optind 3 : %s\n", argv[optind++]);
                    //printf("optind 4 : %s\n", argv[optind++]);
                    //printf("optind 5 : %s\n", argv[optind++]);

                    break;
                }
            case 'b':
                {
                    printf("optarg : %s\n", optarg);
                    printf("b optind : %d\n", optind);
                    printf("b optind 2 : %s\n", argv[optind++]);

                }
                break;
            case 'c':
                {
                    printf("optarg : %s\n", optarg);
                    printf("optind : %d\n", optind);
                }
                break;
            case 'd':
                {
                    printf("optarg : %s\n", optarg);
                    printf("optind : %d\n", optind);
                }
                break;
        }
    }

    printf("\n\n");
    
    for(i=0; i < argc; i++)
    {
        printf("argv[%d] value : %s\n", i, argv[i]);
    }
    printf("%d\n",optind);

    return 0;
}