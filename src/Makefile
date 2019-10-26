#   -----------------------------------------------------------------
#   Copyright (c) 1989 Lars Fredriksen, Bryan So, Barton Miller
#   All rights reserved
#    
#   This software is furnished under the condition that it may not
#   be provided or otherwise made available to, or used by, any
#   other person.  No title to or ownership of the software is
#   hereby transferred.
#  
#   Any use of this software must include the above copyright notice.
#-----------------------------------------------------------------

# source file names
SRCS =  fuzz.c ptyjig.c
# object file names here
OBJS =  fuzz ptyjig


# Add your own flags for the C compiler.
# CFLAGS = -DDEBUG 
#CFLAGS=  -O


# Don't modify these.
all: fuzz ptyjig
	@echo 'all programs generated'

fuzz:  fuzz.c
	gcc -o fuzz fuzz.c -g -Wall -Werror

ptyjig: ptyjig.c
	gcc -o ptyjig ptyjig.c -g -Wall -Werror

#lint: 
#	lint -hxb -DLINT  $(SRCS) > LINTERRS

clean:
	rm -f $(OBJS) 
