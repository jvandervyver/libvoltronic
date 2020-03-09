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
LIBS =

# add includes
CFLAGS += -I$(IDIR) -Ilib/libserialport -Ilib/libhidapi/hidapi

# define the C source files
SDIR = src
SRCS = $(wildcard $(SDIR)/*.c)

# Object files
ODIR = obj
OBJS = $(patsubst %,$(ODIR)/%,$(notdir $(SRCS:.c=.o)))

# Directives

default: no_default

no_default:
	@echo "Different compile options exist (ie. make serial)"
	@echo "  serial - Serial port only"
	@echo "  usb_default - USB support in Mac, Windows, FreeBSD"
	@echo "  hidraw - USB support in Linux using hidraw"
	@echo "  libusb - USB support using libUSB"
	@echo "  serial_usb_default - serial & usb_default"
	@echo "  serial_hidraw - serial & hidraw"
	@echo "  serial_libusb - serial & libusb"

serial: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lserialport
	$(CP) $@ voltroniclib
	$(RM) $@

usb_default: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lhidapi
	$(CP) $@ voltroniclib
	$(RM) $@

serial_usb_default: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lserialport -lhidapi
	$(CP) $@ voltroniclib
	$(RM) $@

hidraw: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lhidapi-hidraw
	$(CP) $@ voltroniclib
	$(RM) $@

serial_hidraw: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lserialport -lhidapi-hidraw
	$(CP) $@ voltroniclib
	$(RM) $@

libusb: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lhidapi-libusb
	$(CP) $@ voltroniclib
	$(RM) $@

serial_libusb: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lserialport -lhidapi-libusb
	$(CP) $@ voltroniclib
	$(RM) $@

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	$(RM) $(ODIR)/*.o *~ core voltroniclib $(INCDIR)/*~ 
