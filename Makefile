# define the C compiler to use
CC = gcc
CHECK=checkmk

# define any compile-time flags
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Wmissing-prototypes -Wshadow -O3 -flto -fomit-frame-pointer

# define any directories containing header files other than /usr/include
#
IDIR = include
INCLUDES = -I$(IDIR) -Ilib/libhidapi/hidapi/

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -Llib/libserialport/

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = -lserialport

ifeq '$(findstring ;,$(PATH))' ';'
	detected_OS := Windows
else
	detected_OS := $(shell uname 2>/dev/null || echo Unknown)
	detected_OS := $(patsubst CYGWIN%,Cygwin,$(detected_OS))
	detected_OS := $(patsubst MSYS%,MSYS,$(detected_OS))
	detected_OS := $(patsubst MINGW%,MSYS,$(detected_OS))
endif

ifeq ($(detected_OS),Windows)
	HIDAPI_DIR = lib/libhidapi/windows
else ifeq ($(detected_OS),Darwin)
	HIDAPI_DIR = lib/libhidapi/mac
	LFLAGS += -framework IOKit -framework CoreFoundation
else ifeq ($(detected_OS),FreeBSD)
	HIDAPI_DIR = lib/libhidapi/libusb
else ifeq ($(detected_OS),NetBSD)
    HIDAPI_DIR = lib/libhidapi/libusb
else ifeq ($(detected_OS),DragonFly)
    HIDAPI_DIR = lib/libhidapi/libusb
else
	HIDAPI_DIR = lib/libhidapi/linux
endif

# define the C source files
SDIR = src
SRCS =	$(wildcard $(SDIR)/*.c) \
		$(HIDAPI_DIR)/hid.c

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