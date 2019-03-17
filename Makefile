# define the C compiler to use
CC = gcc
CHECK=checkmk

# define any compile-time flags
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Wmissing-prototypes -Wshadow -O3 -flto -fomit-frame-pointer

# define any directories containing header files other than /usr/include
#
IDIR = include
INCLUDES = -I$(IDIR)

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -Llib

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = #-lmylib -lm

# define the C source files
SDIR = src
SRCS = $(wildcard $(SDIR)/*.c)

TDIR = tst
TSTS = $(wildcard $(TDIR)/*.check)

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o)

# define the executable file 
MAIN = voltroniclib

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean test

default: $(MAIN)

$(MAIN): $(OBJS) 
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

test:
		$(CHECK) 

clean:
		$(RM) $(OBJS) *~ $(MAIN)

depend: $(SRCS)
		makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it