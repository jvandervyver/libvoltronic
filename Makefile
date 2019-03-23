# define the C compiler to use
CC = gcc
CP = cp -f

# define any compile-time flags
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Wmissing-prototypes -Wshadow -O3 -flto -fomit-frame-pointer

# define any directories containing header files other than /usr/include
IDIR = include
DEPS = $(wildcard $(IDIR)/*.h)

# define any libraries
LDIR = lib
LIBS = -lserialport

# add includes
CFLAGS += -I$(IDIR) -Ilib/libserialport/ -Ilib/libhidapi/hidapi

# define the C source files
SDIR = src
SRCS = $(wildcard $(SDIR)/*.c)

# Object files
ODIR = obj
OBJS = $(patsubst %,$(ODIR)/%,$(notdir $(SRCS:.c=.o)))

# Directives

default: usb_default

usb_default: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lhidapi
	$(CP) $@ voltroniclib
	$(RM) $@

hidraw: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lhidapi-hidraw
	$(CP) $@ voltroniclib
	$(RM) $@

libusb: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lhidapi-libusb
	$(CP) $@ voltroniclib
	$(RM) $@

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	$(RM) $(ODIR)/*.o *~ core voltroniclib $(INCDIR)/*~ 
