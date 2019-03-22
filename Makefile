# define the C compiler to use
CC = gcc

# define any compile-time flags
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Wmissing-prototypes -Wshadow -O3 -flto -fomit-frame-pointer

# define any directories containing header files other than /usr/include
IDIR = include
DEPS = $(wildcard $(IDIR)/*.h)

# define any libraries
LDIR = lib
LIBS = -lserialport -lhidapi

# add includes
CFLAGS += -I$(IDIR) -Ilib/libserialport/ -Ilib/libhidapi/hidapi

# define the C source files
SDIR = src
SRCS = $(wildcard $(SDIR)/*.c)

# Object files
ODIR = obj
OBJS = $(patsubst %,$(ODIR)/%,$(notdir $(SRCS:.c=.o)))

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

default: voltroniclib

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

voltroniclib: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	$(RM) $(ODIR)/*.o *~ core $(INCDIR)/*~ 
