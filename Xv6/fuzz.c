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
 *  Fuzz generator
 *
 *  Usage: fuzz [-0] [-a] [-d delay] [-o file] [-r file] [-l [lll]] 
 *              [-p] [-s sss] [-e epilog] [-x] [n]
 *
 *  Generate n random characters to output.
 *
 *  Options:
 *
 *      n    Length of output, usually in byte. For -l, in # of strings.
 *
 *     -0    NULL (0 byte) characters included
 *
 *     -a    all ASCII character (default)
 *
 *     -d delay
 *           Delay for "delay" seconds between characters
 *
 *     -o file
 *           Record characters in "file"
 *
 *     -r file
 *           Replay characters in "file"
 *
 *     -l    random length LF terminated strings (lll max. default 255)
 *
 *     -p    printable ASCII only
 *
 *     -s    use sss as random seed
 *
 *     -e    send "epilog" after all random characters
 *
 *     -x    print the random seed as the first line
 *
 *  Defaults:
 *           fuzz -a 
 *           
 *
 *  Authors:
 *           Lars Fredriksen, Bryan So
 */

static char *progname = "fuzz";

#include "types.h"
#include "user.h"
#include "fcntl.h"

#define SWITCH '-'

/*
 * Global flags 
 */
int     flag0 = 0;
int     flaga = 1;		/* 0 if flagp */
unsigned flagd = 0;
int     flagl = 0;
int     flags = 0;
int     flage = 0;
int     seed = 0;
int     flagn = 0;
int     flagx = 0;
int     flago = 0;
int     flagr = 0;
int     length = 0;
char    epilog[1024];
char   *infile, *outfile;
int   in, out;
static unsigned long next = 1;

void usage();
void init();
void replay();
void fuzz();
void putch(int i);
void fuzzchar(int m, int h);
void fuzzstr(int m, int h);
void myputs(char *s);
// int oct2dec(int i);
int rand();
void srand(uint seed);

int main(int argc, char** argv)
{
     /*
      * Parse command line 
      */
     while (*(++argv) != 0)
	  if (**argv != SWITCH) {	/* Not a switch, must be a length */
	       //if (sscanf(*argv, "%d", &length) != 1)
	       if(*argv == 0)
	           usage();
               length = atoi(*argv);
	       flagn = 1;
	  } else {		/* A switch */
	       switch ((*argv)[1]) {
	       case '0':
		    flag0 = 1;
		    break;
	       case 'a':
		    flaga = 1;
		    break;
	       case 'd':
		    argv++;
		    /*if (sscanf(*argv, "%f", &f) != 1)
			 usage();*/
		    if(*argv == 0)
                        usage();
		    flagd = atoi(*argv);
		    break;
	       case 'o':
		    flago = 1;
		    argv++;
		    outfile = *argv;
		    break;
	       case 'r':
		    flagr = 1;
		    argv++;
		    infile = *argv;
		    break;
	       case 'l':
		    flagl = 255;
		    if (argv[1] != 0 && argv[1][0] != SWITCH) {
			 argv++;
			 /*if (sscanf(*argv, "%d", &flagl) != 1 || flagl <= 0)
			      usage();*/
		    }
		    break;
	       case 'p':
		    flaga = 0;
		    break;
	       case 's':
		    argv++;
		    flags = 1;
		    /*if (sscanf(*argv, "%d", &seed) != 1)
			 usage();*/
		    if(*argv == 0)
                        usage();
		    seed = atoi(*argv);
		    break;
	       case 'e':
		    argv++;
		    flage = 1;
		    if (*argv == 0)
			 usage();
		    // sprintf(epilog, "%s", *argv);
		    int length = strlen(*argv);
		    for(int i = 0; i < length; i++){
		        epilog[i] = (*argv)[i];
		    }
		    break;
	       case 'x':
		    flagx = 1;
		    break;
	       default:
		    usage();
	       }
	  }

     init();
     if (flagr)
	  replay();
     else
	  fuzz();
     myputs(epilog);

     if (flago)
         close(out);
	 /* if (fclose(out) == EOF) {
	       printf(2, "%s\n", outfile);
	       exit();
	  }*/
     if (flagr)
	 close(in);
	 /* if (fclose(in) == EOF) {
	       printf(2, "%s\n", infile);
	       exit();
	  }*/
     exit();
}


void usage()
{
     printf(1, "Usage: fuzz [-x] [-0] [-a] [-l [strlen]] [-p] [-o outfile]\n");
     printf(1, "            [-r infile] [-d delay] [-s seed] [-e \"epilog\"] [len]\n");
}


/*
 * Initialize random number generator and others 
 */
void init()
{
     /*
      * Init random numbers 
      */
     if (!flags)
	  seed = (int) (uptime() % 37);
     srand(seed);

     /*
      * Random length if necessary 
      */
     if (!flagn)
	  length = rand() % 100000;

     /*
      * Open data files if necessary 
      */
     if (flago)
	 if((out = open(outfile, O_CREATE|O_RDWR)) < 0){
             printf(1, "%s\n", outfile);
             exit();
         }
    
     if (flagr) {
	 if((in = open(infile, 0)) < 0){
             printf(1, "%s\n", infile);
             exit();
         }
     } else if (flagx) {
	  printf(1, "%d\n", seed);
	  /*if (fflush(stdout) == EOF) {
	       printf(1, "%s\n", progname);
	       exit();
          }
	  if (flago) {
	       printf(1, "%d\n", seed);
	       if (fflush(out) == EOF) {
		     printf(1, "%s\n", outfile);
		     exit();
               }
	  }*/
     }
}


/*
 * Replay characters in "in" 
 */
void replay()
{
     int     c;

     // while ((c = gets(in, 1)) != EOF)
     while(read(in, &c, 1) > 0)
	  putch(c);
}


/*
 * Decide th effective range of the random characters 
 */
void fuzz()
{
     int     m, h;

     /*
      * Every random character is of the form c = rand() % m + h 
      */
     h = 1;
     m = 255;			/* Defaults, 1-255 */
     if (flag0) {
	  h = 0;
	  m = 256;		/* All ASCII, including 0, 0-255 */
     }
     if (!flaga) {
	  h = 32;
	  m = 95 + (flag0!=0); 	/* Printables, 32-126 */
     }
     if (flagl)
	  fuzzstr(m, h);
     else
	  fuzzchar(m, h);
}


/*
 * Output a character to standard out with delay 
 */
void putch(int i)
{
     char    c;

     c = (char) i;
     if (write(1, &c, 1) != 1) {
	  printf(1, "%s\n", progname);
	  if (flagr){
	      close(in);
	      unlink(infile);
	  }
	  if (flago){
	      close(out);
              unlink(outfile);
	  }
	  exit();
     }
     if (flago){
	 if(write(out, &c, 1) != 1){
             printf(1, "%s\n", outfile);
             exit();
         }
     }
     if (flagd)
	  sleep(flagd);
}


/*
 * Make a random character 
 */
void fuzzchar(int m, int h)
{
     int     i,c;

     for (i = 0; i < length; i++) {
	  c = (int) (rand() % m) + h;
	  if (flag0 && !flaga && c == 127)
	       c = 0;
	  putch(c);
     }
}


/*
 * make random strings 
 */
void fuzzstr(int m, int h)
{
     int     i, j, l, c;

     for (i = 0; i < length; i++) {
	  l = rand() % flagl;	/* Line length  */
	  for (j = 0; j < l; j++) {
	       c = (int) (rand() % m) + h;
	       if (flag0 && !flaga && c == 127)
	            c = 0;
	       putch(c);
	  }
	  putch('\n');
     }
}


/*
 * Output the "epilog" with C escape sequences 
 */
void myputs(char *s)
{
     // int     c;

     while (*s != '\0') {
	  if (*s == '\\') {
	       switch (*++s) {
	       case 'b':
		    putch('\b');
		    break;
	       case 'f':
		    putch('\f');
		    break;
	       case 'n':
		    putch('\n');
		    break;
	       case 'r':
		    putch('\r');
		    break;
	       case 't':
		    putch('\r');
		    break;
	       case 'v':
		    putch('\v');
		    break;
	       /*case 'x':
		    s++;
		    (void) sscanf(s, "%2x", &c);
		    putch(c);
		    s++;
		    break;*/
	       default:
		    /*if (isdigit(*s)) {
			 (void) sscanf(s, "%3d", &c);
			 putch(oct2dec(c));
			 for (c = 0; c < 3 && isdigit(*s); c++)
			      s++;
			 s--;
		    } else
			 putch(*s);*/
		    putch(*s);
		    break;
	       }
	  } else
	       putch(*s);
	  s++;
     }
}


/*
 * Octal to Decimal conversion, required by myputs() 
 */
/*int oct2dec(int i)
{
     char    s[8];
     int     r = 0;

     sprintf(s, "%d", i);
     for (i = 0; i < strlen(s); i++)
	  r = r * 8 + (s[i] - '0');

     return r;
}*/

int rand(){
    next = next * 1103515245 + 12345;
    return((uint)(next/65536) % 32768);
}

void srand(uint seed){
    next = seed;
}
