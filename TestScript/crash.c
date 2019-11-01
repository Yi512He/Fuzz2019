#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

    int crash()
    {
            char *xxx = "crash!!";
            xxx[1] = 'D'; // 写只读存储区!
            return 2;
    }
 
    int foo()
    {
            return crash();
    }
 
    int main()
    {
	    pid_t id = getpid();
	    printf("pid is: %d\n", id);
            return foo();
    }
