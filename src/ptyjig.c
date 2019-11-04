/* Copyright (c) 1989 Lars Fredriksen, Bryan So, Barton Miller
 * All rights reserved
 *  
 * This software is furnished under the condition that it may not
 * be provided or otherwise made available to, or used by, any
 * other person.  No title to or ownership of the software is
 * hereby transferred.
 *
 * Any use of this software must include the above copyright notice.
 */


// Copyright (c) 1989 by Lars Fredriksen, Bryan So and Barton Miller.
// All rights reserved.


/*-
 *  pty -- Super pipe for piping output to Unix utilities.
 *
 *  Usage:  pty [-esx] [-i filei] [-o fileo] [-d ddd] [-t ttt] 
 *              [-w www] cmd [args]
 *
 *  Run Unix command "cmd" with arguments "args" in background, piping
 *  standard input to "cmd" as its input and prints out "cmd"'s output
 *  to stdout.  This program sets up pseudo-terminal pairs, so that 
 *  it can be used to pipe input to programs that read directly from
 *  tty.
 *
 *  -e suppresses sending of EOF character after stdin exhausted
 *
 *  -s suppresses interrupts.
 *
 *  -x suppresses the standard output.
 *
 *  -i specifies a file to which the standard input is saved.
 *
 *  -o specifies a file to which the standard output is saved.
 *
 *  -d specifies a keystroke delay in seconds (floating point accepted.)
 *
 *  -t specifies a timeout interval.  The program will exit if the
 *     standard input is exhausted and "cmd" does not send output
 *     for "ttt" seconds.
 *
 *  -w specifies another delay parameter. The program starts to send
 *     input to "cmd" after "www" seconds.
 *
 *  Defaults:
 *             -i /dev/nul -o /dev/nul -d 0 -t 2
 *
 *  Examples:
 *         
 *     pty -o out -d 0.2 -t 10 vi text1 < text2
 *
 *        Starts "vi text1" in background, typing the characters in 
 *        "text2" into it with a delay of 0.2sec between each char-
 *        acter, and save the output by "vi" to "out".  The program
 *        ends when "vi" stops outputting for 10 seconds.
 *
 *     pty -i in -o out csh
 *
 *        Behaves like "script out" except the keystrokes typed by
 *        a user are also saved into "in".
 *
 *  Authors:
 *              Bryan So, Lars Fredriksen
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sgtty.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define CHILD 0

char    flage = 1;
int     flags = 0;
int     flagx = 0;
int     flagi = 0;
int     flago = 0;
unsigned flagt = 2000000;	/* Timeout interval in useconds */
unsigned flagw = 0;             /* Starting wait in useconds */
unsigned flagd = 0;		/* Delay between keystrokes in useconds */

char   *namei;
char   *nameo;
FILE   *filei;
FILE   *fileo;

int rpid = -1,wpid = -1,epid = -1; /* pids for the reader, writer and exec */

int     tty, pty; /* tty and pty file descriptors */
int ctty;
char    ttynames[40];
char    *progname;

struct  mesg {
        char    *iname;
        char    *pname;
} mesg[] = {
        0,      0,
        "HUP",  "Hangup",
        "INT",  "Interrupt",
        "QUIT", "Quit",
        "ILL",  "Illegal instruction",
        "TRAP", "Trace/BPT trap",
        "IOT",  "IOT trap",
        "EMT",  "EMT trap",
        "FPE",  "Floating exception",
        "KILL", "Killed",
        "BUS",  "Bus error",
        "SEGV", "Segmentation fault",
        "SYS",  "Bad system call",
        "PIPE", "Broken pipe",
        "ALRM", "Alarm clock",
        "TERM", "Terminated",
        "URG",  "Urgent I/O condition",
        "STOP", "Stopped (signal)",
        "TSTP", "Stopped",
        "CONT", "Continued",
        "CHLD", "Child exited",
        "TTIN", "Stopped (tty input)",
        "TTOU", "Stopped (tty output)",
        "IO",   "I/O possible",
        "XCPU", "Cputime limit exceeded",
        "XFSZ", "Filesize limit exceeded",
        "VTALRM","Virtual timer expired",
        "PROF", "Profiling timer expired",
        "WINCH","Window size changed",
        0,      "Signal 29",
        "USR1", "User defined signal 1",
        "USR2", "User defined signal 2",
        0,      "Signal 32"
};

void sigchld();
void clean();
void sigwinch();
void clean_init(int sigsum);
void clean_quit(int sigsum);
void clean_term(int sigsum);
void doreader();
void dowriter();
void gettty();
void fixtty();
void unfixtty();
void setup_pty();
void setup_tty();
void done();
void execute(char **argv);
void writer();
void reader();
void reader_done(int sigsum);
void writer_done(int sigsum);
void execute_done(int sigsum);
void fatal();
void usage();

// added by zmx
void clean_int(int sigsum);
// void clean_quit(int sigsum);
// void clean_term(int sigsum);


int main(int argc, char **argv, char **envp)
{
     int     c;
     float   f;
     extern int optind;
     extern char *optarg;
     // int clean_int(),clean_quit(),clean_term();
     void sigwinch(),sigchld();

     while ((c = getopt(argc, argv, "esxi:o:t:d:w:")) != EOF) {
	  switch (c) {
	  case 'e':
	       flage = 0;
	       break;
	  case 's':
	       flags++;
	       break;
	  case 'x':
	       flagx++;
	       break;
	  case 'i':
	       flagi++;
	       namei = optarg;
	       break;
	  case 'o':
	       flago++;
	       nameo = optarg;
	       break;
	  case 'd':
	       if (sscanf(optarg, "%f", &f) < 1)
		    usage();
	       flagd = (unsigned) (f * 1000000.0);
	       break;
	  case 't':
	       if (sscanf(optarg, "%f", &f) < 1)
		    usage();
	       flagt = (unsigned) (f * 1000000.0);
	       break;
	  case 'w':
	       if (sscanf(optarg, "%f", &f) < 1)
		    usage();
	       flagw = (unsigned) (f * 1000000.0);
	       break;
	  default:
	       usage();
	  }
     }

     /* Now, "optind" points to the command */
     if (!argv[optind])
	  usage();

     if (flagi) {
	  filei = fopen(namei, "wb");
	  if (filei == NULL) {
	       perror(namei);
	       exit(1);
	  }
     }
     if (flago) {
	  fileo = fopen(nameo, "wb");
	  if (fileo == NULL) {
	       perror(nameo);
	       exit(1);
	  }
     }

     gettty();

     /* open an arbitrary pseudo-terminal pair  */
     setup_pty();

     signal(SIGCHLD,sigchld); 

     /* fork and execute test program with arguments */
     progname = argv[optind]; 
     execute((char **) &argv[optind]);

     fixtty();

     signal(SIGWINCH,sigwinch); 
     rpid = getpid(); 

     dowriter();

     signal(SIGQUIT,clean_quit); 
     signal(SIGTERM,clean_term); /* Sig's are put here instead of above to */
     signal(SIGINT,clean_int);   /* avoid invoking clean_ twice */
				
     doreader();

     while (1)  /* Wait for sigchld to exit */
	  ;
}

void sigchld(int sigsum)
{
     int pid;
     //union wait status;
     int status;

#ifdef DEBUG
puts("sigchld\r");
#endif
     /* Guarantee to return since a child is dead */
     pid = wait3(&status,WUNTRACED,0);  
     if (pid) {
#ifdef DEBUG
	  printf("pid = %d\r\n",pid);
	  printf("status = %d %d %d %d %d\r\n",
		  WTERMSIG(status), WCOREDUMP(status),WEXITSTATUS(status),
		  WIFSTOPPED(status),WSTOPSIG(status));
#endif
          if (WSTOPSIG(status) == SIGTSTP) {
	       kill(pid,SIGCONT);
	       return;
	  }
	  signal(SIGINT,SIG_DFL);
	  signal(SIGQUIT,SIG_DFL); 
	  signal(SIGTERM,SIG_DFL); 
	  signal(SIGWINCH,SIG_DFL); 
	  signal(SIGCHLD,SIG_IGN);
	  done();
          if (pid != epid) {
#ifdef DEBUG
	       printf("Somebody killed my child\r\n");
               printf("kill epid = %d\r\n",epid);
#endif
	       kill(epid,SIGKILL);   /* kill the exec too */
	       kill(rpid,WTERMSIG(status)); /* use the same method to suicide */
	  }
	  kill(epid,SIGKILL); /* Just to make sure it is killed */
	  if (pid != wpid && wpid != -1)
#ifdef DEBUG
printf("kill wpid = %d\r\n",wpid),
#endif
	       kill(wpid,SIGKILL);
	  if (WTERMSIG(status))
	       fprintf(stderr,"%s: %s%s\n",progname,
		   mesg[WTERMSIG(status)].pname,
		   WCOREDUMP(status)? " (core dumped)": "");

          /* If process terminates normally, return its retcode */
	  /* If abnormally, return termsig.  This is not exactly */
	  /* the same as csh, since the csh method is not too obvious */

	  exit(WTERMSIG(status)? WTERMSIG(status): WEXITSTATUS(status));
     }

     exit(0);
}

void clean()
{
#ifdef DEBUG
puts("clean()\r");
#endif
     signal(SIGCHLD,SIG_IGN); /* Not necessary for sigchld to take over */

     /* must close files, and kill all running processes */
     if (epid != -1)
#ifdef DEBUG
printf("kill epid = %d\r\n",epid),
#endif
	  kill(epid,SIGKILL);
     if (wpid != -1)
#ifdef DEBUG
printf("kill wpid = %d\r\n",wpid),
#endif
	  kill(wpid,SIGKILL);

     done();
}

/* Handle window size change */
void sigwinch(int sigsum)
{
     struct winsize ws;

     ioctl(0, TIOCGWINSZ, &ws);
     ioctl(pty, TIOCSWINSZ, &ws);
     kill(epid, SIGWINCH);
}

/* Handle user interrupt */
void clean_int(int sigsum)
{
#ifdef DEBUG
puts("sigint");
#endif
     signal(SIGINT, SIG_DFL);
     clean();
     kill(rpid, SIGINT);
}

/* Handle quit */
void clean_quit(int sigsum)
{
#ifdef DEBUG
puts("sigquit\r");
#endif
     clean();
     signal(SIGQUIT,SIG_DFL);
     kill(rpid,SIGQUIT);
}

/* Handle user terminate */
void clean_term(int sigsum)
{
#ifdef DEBUG
puts("sigterm\r");
#endif
     clean();
     signal(SIGTERM,SIG_DFL);
     kill(rpid,SIGTERM);
}


void doreader()
{
     reader();
}

void dowriter()
{
     if ((wpid = fork()) == -1) {
	  perror("dowriter(): fork:");
	  exit(1);
     } 

     if (wpid == CHILD) {
	  wpid = getpid();
	  writer();
     }
     
#ifdef DEBUG
printf("wpid = %d\r\n",wpid);
#endif

}


struct sgttyb oldsb; /* terminal descriptors */
struct sgttyb gb;
struct tchars gtc;
struct ltchars glc;
struct winsize gwin;
int     glb, gl;

void gettty()
{
     int ctty;

     if ((ctty = open("/dev/tty",O_RDWR)) >= 0) {
	  ioctl(ctty,TIOCGETP,(char*)&oldsb);
	  ioctl(ctty,TIOCGETC,(char*)&gtc);
	  ioctl(ctty, TIOCGETD, (char *) &gl);
	  ioctl(ctty, TIOCGLTC, (char *) &glc);
	  ioctl(ctty, TIOCLGET, (char *) &glb);
	  ioctl(ctty, TIOCGWINSZ, (char *) &gwin);
	  close(ctty);
     }
}


void fixtty()
{
     struct sgttyb b;
     struct tchars tc;
     int ctty;

     /*
      * Make the original terminal characteristics RAW and CBREAK (i.e.
      * return every character when typed) and not ECHO. 
      */

     if (flage) flage = gtc.t_eofc;
     b = oldsb;
     b.sg_flags |= RAW | CBREAK;
     b.sg_flags &= ~ECHO;
     ioctl(0, TIOCSETP, (char *) &b);
}

void unfixtty()
{
     ioctl(0, TIOCSETP, (char*)&oldsb);
}

/*
 * Opens a master pseudo-tty device. 
 */
void setup_pty()
{
     char    c;
     int     i;
     struct stat stb;

     /*
      * Make up the pseudo-tty names, namely /dev/ptyp0.../dev/ptysf 
      */
     for (c = 'p'; c <= 's'; c++)
	  for (i = 0; i < 16; i++) {
	       sprintf(ttynames, "/dev/pty%c%x", c, i);
	       if (stat(ttynames, &stb) < 0)
		    fatal();
	       if ((pty = open(ttynames, O_RDWR)) > 0) {
		    /*
		     * Check for validity of the other side 
		     */
		    ttynames[5] = 't';
		    if (access(ttynames, R_OK | W_OK) == 0)
			 return;
		    else
			 close(pty);
	       }
	  }
     fatal();
     exit(1);
}


/*
 * Opens the slave device.  The device name is already in "ttynames" (put in
 * there by setup_pty(). 
 */
void setup_tty()
{
     struct sgttyb b;
     int ctty;

     if ((ctty = open("/dev/tty",O_RDWR)) >= 0) {
	  ioctl(ctty, TIOCNOTTY,0);  
/*	  close(ctty);      /* prevent character lost by not closing it */
     }

     /*
      * Reopen "ttynames" as the control terminal 
      */
     tty = open(ttynames, O_RDWR);
     if (tty < 0) {
	  perror(ttynames);
	  exit(1);
     }
     /*
      * Transfer the capabilities to the new tty 
      */
     b = oldsb;

     /*
      * In particular, when coming from a pipe, do not modify '\r' to '\r\n' 
      */
     if (!isatty(0))
	  b.sg_flags &= ~CRMOD;

     ioctl(tty, TIOCSETP, (char *) &b);
     ioctl(tty, TIOCSETC, (char *) &gtc);
     ioctl(tty, TIOCSETD, (char *) &gl);
     ioctl(tty, TIOCSLTC, (char *) &glc);
     ioctl(tty, TIOCLSET, (char *) &glb);
     ioctl(tty, TIOCSWINSZ, (char *) &gwin);
}



/*
 * Unfix the above and exit where we are done. 
 */
void done()
{
#ifdef DEBUG
     puts("done()\r");
#endif
     /*
      * Close output files if opened. 
      */
     if (flagi) {
	  fclose(filei);
	  flagi = 0;
     }
     if (flago) {
	  fclose(fileo);
	  flago = 0;
     }
     /*
      * Reset the ECHO ability; otherwise, the shell does not echo any typed
      * characters! 
      */
     unfixtty();
}

int executing = 1;

/*
 * Fork off a copy and execute "argv".  Before executing, assign "tty" to
 * stdin, stdout and stderr, so that the output of the child program can be
 * recorded by the other end of "tty". 
 */
void execute(char **argv)
{
     // int execute_done();

     signal(SIGUSR1,execute_done);
     if ((epid = fork()) == -1) {
	  perror("execute(): fork");
	  exit(1);
	  }
     if (epid == CHILD) {
     	  int fstdin,fstdout,fstderr;

          fstdin = dup(0);
	  fstdout = dup(1);
     	  fstderr = dup(2); /* save a copy in case exec failed */
	  setup_tty();
	  dup2(tty, 0);		/* copy tty to stdin  */
	  dup2(tty, 1);		/* copy tty to stdout  */
	  dup2(tty, 2);		/* copy tty to stderr  */
	  close(tty);
	  if (flags) {		/* suppress signals if -s present */
	       signal(SIGINT, SIG_IGN);
	       signal(SIGQUIT, SIG_IGN);
	       signal(SIGTSTP, SIG_IGN);
	  }
	  kill(getppid(),SIGUSR1);
	  execvp(argv[0], argv);

	  /* if it ever gets here, error when executing "cmd"  */
	  dup2(fstdin,0);
	  dup2(fstdout,1);
	  dup2(fstderr,2);
	  unfixtty();
	  perror(argv[0]);
	  exit(1);
     }
     while (executing) /* let child run until it gives signal */
	  ;
     usleep(flagw);
#ifdef DEBUG
     printf("epid = %d\r\n",epid);
#endif
}


/* A flag indicating status of the writer  */
int     writing = 1;

/*
 * Read from stdin and send everything character read to "pty".  Record the
 * keystrokes in "filei" if -i flag is on. 
 */
void writer()
{
     char    c;

     /*
      * Read from keyboard continuously and send it to "pty" 
      */
     while (read(0, &c, 1) == 1) { 
	  if (write(pty, &c, 1) != 1)
	       break;
	  if (flagi) {
	       if (c == '\r') 
		    c = '\n';
	       if (write(fileno(filei), &c, 1) != 1) {
		    perror(namei);
		    break;
	       }
	  }
	  if (flagd)
	       usleep(flagd);
     }
     if (flage)
	  (void)write(pty, &flage, 1);

#ifdef DEBUG
     puts("end writer\r");
#endif
     kill(rpid,SIGUSR1); /* tell reader to quit if no more char from exec */
     while (1) /* wait until parent to kill me */
	  ;
}


/*
 * Read from "pty" and send it to stdout 
 */
void reader()
{
     char    c[BUFSIZ];
     int     i;
     // int reader_done();
     // int writer_done();

     /*
      * Continuously read from "pty" until exhausted.  Write every character
      * to stdout if -x flag is not present, and to "fileo" if -o flag is on.   
      */
     signal(SIGALRM,reader_done);
     signal(SIGUSR1,writer_done);
     while ((i = read(pty, c, sizeof(c))) > 0) {
	  if (!flagx)
	       if (write(1, c, i) != i)
		    exit(1);
	  if (flago)
	       if (write(fileno(fileo), c, i) != i) {
		    perror(nameo);
		    exit(1);
	       }
	  /*
	   * The following "if" essentially means when "writer" is done, and
	   * there is no more keystroke coming from "pty" wait for "flagt"
	   * seconds and quit.  If during this wait, a character comes from
	   * "pty", then the wait is set back. 
	   */
	  if (!writing) 
	       ualarm(flagt, 0);
     }

#ifdef DEBUG
     puts("end reader\r");
#endif

     reader_done(666);
}

void reader_done(int sigsum)
{
     sleep(1); /* Let epid die naturally */
#ifdef DEBUG
printf("kill epid = %d\r\n",epid);
#endif
     kill(epid,SIGKILL); /* If it doesn't, kill it */
}

void writer_done(int sigsum)
{
     writing = 0;
     ualarm(flagt,0);
}

void execute_done(int sigsum)
{
     executing = 0;
}

/*
 * No more ptys available 
 */
void fatal()
{
     fprintf(stderr, "No pty's available\n");
     exit(1);
}


/*
 * Display usage 
 */
void usage()
{
     fprintf(stderr, "Usage: pty [-esx] [-i file] [-o file] [-d delay] \
[-t timeout] [-w wait] command [arg]\n");

     exit(1);
}
