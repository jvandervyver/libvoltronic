# define the C compiler to use
CC := gcc

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

SHARED_LIBS :=
SHARED_LIBS_SERIAL :=
SHARED_LIBS_HID :=
SERIAL_BINARY := $(LDIR)/libserialport/.libs/libserialport.a
HID_BINARY :=

# Object files shared by all directives
SHARED_OBJS = $(ODIR)/voltronic_crc.o $(ODIR)/voltronic_dev.o

# Operating specific overrides
ifeq ($(OS),Windows_NT)
  SHARED_LIBS := -lkernel32 -lhid -lsetupapi -lwsock32 -lws2_32
  HID_BINARY := $(LDIR)/hidapi/windows/.libs/libhidapi.a
  ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
    CC := x86_64-w64-mingw32-gcc
  else
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
      CC := x86_64-w64-mingw32-gcc
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
      CC := i686-w64-mingw32-gcc
    endif
  endif
else
  OS_FOUND := false
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Linux)
  	SHARED_LIBS_HID := -ludev
  	HID_BINARY := $(LDIR)/hidapi/linux/.libs/libhidapi-hidraw.a
  	OS_FOUND := true
  endif
  ifeq ($(UNAME_S),Darwin)
    SHARED_LIBS = -framework IOKit -framework Foundation
    HID_BINARY := $(LDIR)/hidapi/mac/.libs/libhidapi.a
    OS_FOUND := true
  endif
  ifeq ($(OS_FOUND),false)
  	HID_BINARY := $(LDIR)/hidapi/libusb/.libs/libhidapi.a
  endif
endif

# Directives
default:
	@echo "Different compile options exist using different underlying hardware and libraries to communicate with the hardware"
	@echo ""
	@echo "  libserialport - Serial port using libserialport"
	@echo "  hidapi - USB support using HIDApi"
	@echo ""
	@echo "Usage: make libserialport; make hidapi; etc."

libserialport: $(SHARED_OBJS) $(ODIR)/serial_main.o $(ODIR)/voltronic_dev_serial_libserialport.o $(SERIAL_BINARY)
	$(CC) -o $@ $^ $(CFLAGS) $(SHARED_LIBS) $(SHARED_LIBS_SERIAL)

hidapi: $(SHARED_OBJS) $(ODIR)/usb_main.o $(ODIR)/voltronic_dev_usb_hidapi.o $(HID_BINARY)
	$(CC) -o $@ $^ $(CFLAGS) $(SHARED_LIBS) $(SHARED_LIBS_HID)

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	$(RM) $(ODIR)/*.o *~ libserialport libserialport.exe hidapi hidapi.exe $(INCDIR)/*~ 
