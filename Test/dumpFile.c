#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv){
    printf("%d\n", getpid());

    *(int*)0 = 0; 

    return 0;

}

