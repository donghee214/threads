#include "common.h"

int
main(int argc, char **argv)
{
    int loop;
    
    for(loop = 1; loop < argc; loop++){
        printf(argv[loop]);
        printf("\n"); 
    }
        

    return 0;
}
