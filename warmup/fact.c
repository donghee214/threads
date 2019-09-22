#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int factorial(int num){
    if(num == 0){
        return 1;
    };
    return num * factorial(num - 1);
};

int
main(int argc, char **argv)
{   
    int value;
    char charValue[strlen(argv[1])];
    if(argv[1][0] == '-' || argv[1][0] == '0'){
        printf("Huh?\n");
        return 0;
    }
    value = atoi(argv[1]);
    sprintf(charValue, "%d", value);
    if(strcmp(charValue, argv[1]) != 0){
        printf("Huh?\n");
    }
    else if(value > 12){
        printf("Overflow\n");
    }
    else{
        printf("%d\n", factorial(value));
    }
    return 0;
}
