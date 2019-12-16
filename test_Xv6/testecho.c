#include "types.h"
#include "stat.h"
#include "user.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  int pid = fork();

  if(pid < 0){
    printf(1, "wrong\n");
    exit();
  }

  if(pid == 0){
    char *arg[3];
    arg[0] = "echo";
    arg[1] = "lalala";
    arg[2] = 0;
    exec("echo", arg);

  }

  if(wait() != pid){
    exit();
  }

  exit();
}

void
test(char *cmd, char *t)
{
    int pid = fork();

    if(pid < 0){
        printf(1, "wrong\n");
	exit();
    }

    if(pid == 0){
        char *args[4];
	args[0] = "fuzz"
	args[1] = "<"
        args[2] = t;
	args[3] = 0;
	exec(*args, args);
    }

    if(wait() != pid){
        exit();
    }

    exit();
}
