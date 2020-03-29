# define the C compiler to use
CC = gcc
CP = cp -f

#directories
IDIR = include
LDIR = lib
SDIR = src
ODIR = obj

# define any compile-time flags
CFLAGS = -std=c99 -Werror -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wshadow -O3 -flto -fomit-frame-pointer

# add includes
CFLAGS += -I$(IDIR) -I$(LDIR)/libserialport -I$(LDIR)/hidapi/hidapi -I$(LDIR)/libusb/libusb

# define the C source files
SRCS = $(wildcard $(SDIR)/*.c)

# Object files shared by all directives
SHARED_OBJS = $(ODIR)/time_util.o $(ODIR)/voltronic_crc.o $(ODIR)/voltronic_dev.o 

# Directives
default:
	@echo "Different compile options exist using different underlying hardware and libraries to communicate with the hardware"
	@echo ""
	@echo "  libserialport - Serial port using libserialport"
	@echo "  hidapi - USB support using HIDApi in Mac, Windows, FreeBSD"
	@echo "  hidapi-hidraw - USB support in Linux using HIDApi utilizing HIDRaw"
	@echo "  hidapi-libusb - USB support using HIDApi utilizing LibUSB"
	@echo ""
	@echo "Usage: make libserialport; make hidapi; etc."

libserialport: $(SHARED_OBJS) $(ODIR)/serial_main.o $(ODIR)/voltronic_dev_serial_libserialport.o
	$(CC) -o $@ $^ $(CFLAGS) -lserialport
	$(CP) $@ libvoltronic_libserialport
	$(RM) $@

hidapi: $(SHARED_OBJS) $(ODIR)/usb_main.o $(ODIR)/voltronic_dev_usb_hidapi.o
	$(CC) -o $@ $^ $(CFLAGS) -lhidapi
	$(CP) $@ libvoltronic_hidapi
	$(RM) $@

hidapi-hidraw: $(SHARED_OBJS) $(ODIR)/usb_main.o $(ODIR)/voltronic_dev_usb_hidapi.o
	$(CC) -o $@ $^ $(CFLAGS) -lhidapi-hidraw
	$(CP) $@ libvoltronic_hidapi_hidraw
	$(RM) $@

hidapi-libusb: $(SHARED_OBJS) $(ODIR)/usb_main.o $(ODIR)/voltronic_dev_usb_hidapi.o
	$(CC) -o $@ $^ $(CFLAGS) -lhidapi-libusb
	$(CP) $@ libvoltronic_hidapi_libusb
	$(RM) $@

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	$(RM) $(ODIR)/*.o *~ libvoltronic_libserialport libvoltronic_hidapi libvoltronic_hidapi_hidraw libvoltronic_hidapi_libusb $(INCDIR)/*~ 
