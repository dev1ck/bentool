#include "protocol.h"
#include "usages.h"

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        usage();
    }

    if(bentool_main(argc, argv)<0)
        return -1;

    return 0;
}