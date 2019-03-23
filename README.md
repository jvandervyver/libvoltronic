## libvoltronic
Library to interface with [Voltronic](http://voltronicpower.com) devices like the Axpert &amp; InfiniSolar

## License
This entire library is licenced under GNU GENERAL PUBLIC LICENSE v3

That means if you want to use this library you can, provided your source code is also also open sourced.

**I will not take lightly any use of this implementation in ANY closed source software**

## Description
Devices from Voltronic are shipped with 4 possible hardware interfaces: RS232, USB, Bluetooth & RS485

All the interfaces share the same underlying communication protocol

## Communication protocol
The communication protocol consists of the following format:

**Overall the protocol has the following format:**

`{bytes}{CRC16}{end of input character}`
- **bytes** the actual bytes being sent to the device, generally speaking this is the *"command"*
- **CRC16** common CRC protocol with many implementations online
- **end of input character** character signaling the end of input

### Reserved characters
These characters are reserved
- `\r` (*0x0d*) End of input character
- `(` (*0x28*) Seems to indicate start of input
- `\n` (*0x0a*) No material importance but still reserved

### Bytes
The bytes being sent to the device appear to be simply ASCII in the form of a command

Multiple documents exist listing possible commands
 - [Axpert](https://s3-eu-west-1.amazonaws.com/osor62gd45llv5fcg47yijafsz6dcrjn/HS_MS_MSX_RS232_Protocol_20140822_after_current_upgrade.pdf)
 - [Infini Solar](https://s3-eu-west-1.amazonaws.com/osor62gd45llv5fcg47yijafsz6dcrjn/Infini_RS232_Protocol.pdf)

### CRC
The CRC is used is the [CRC16 XMODEM](https://pycrc.org/models.html#xmodem) variation

**Background**

Multiple methods exist to [generate CRC](https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks).

CRC16 as the name implies contains 16 bits or 2 bytes of data.
It is commonly written as hexadecimal for readability reason, ie> `0x17AD`

Two hexadecimal character represent a single byte so given the example above.
`0x` part simply indicates hexadecimal
`0x17` is the first byte
`0xAD` is the second byte

**Exception**

The **Reserved characters** are not allowed in the CRC.
It appears the device simply expects them to be incremented by 1

So `0x28` becomes `0x29`, `0x0d` becomes `0x0e`, etc.

### End of input character
The `\r` character signals to the device end of input

Regardless of what the device received up to that point `\r` signals to the device end of current input

Once this character is received all input up to that point is taken as the *command* to the device

## Input methods
Devices from Voltronic are shipped with 4 possible hardware interfaces: RS232, USB, Bluetooth & RS485

All input methods share the same bandwidth & latency.
Although it would appear at surface that USB should be faster, no measureable difference exists to device response time and symbol rate.

USB is also an asynchronous protocol and as such could be influenced by other factors slowing it down further

### Simultaneous communication across multiple interfaces
During testing it was found that simultaneous communication across USB & RS232 for example would result in device lockup.
The device keeps operating, but the device will no longer respond to input or produce output.

As such it is adviced to pick an interface and use it exclusively

### RS232
Nothing special to mention here, synchronous protocol with the following configuration:
- **Baud** *2400*
- **Data bits** *8*
- **Stop bits** *1*
- **Parity** *None* (provided by CRC16)

### USB
The device makes use of an [HID interface](https://en.wikipedia.org/wiki/USB_human_interface_device_class).
In Linux the device is presented as a [*HIDRaw* device](https://www.kernel.org/doc/Documentation/hid/hidraw.txt)

It is **not** a USB->Serial

So in Linux for example:

**Ruby:**
```ruby
fd = File.open('/dev/hidraw0', IO::RDWR|IO::NONBLOCK) # May need root, or make the file 666 using udev rules
fd.binmode
fd.sync = true
fd.write("QPI\xBE\xAC\r") # Will write QPI => Returns 6
fd.gets("\r") #=> "(PI30\x9A\v\r"
```

**Python:**
```python
import os, sys
fd = open("/dev/hidraw0", os.O_RDWR|os.O_NONBLOCK)
os.write(fd, "QPI\xBE\xAC\r")
os.read(fd, 512)
```

**Avoiding the need for root**

Make use of [**udev**](https://wiki.debian.org/udev) to specify more broad access:

```bash
# may require root
touch /etc/udev﻿/rules.d/15-voltronic.rules
echo 'ATTRS{idVendor}=="0665", ATTRS{idProduct}=="5161", SUBSYSTEMS=="usb", ACTION=="add", MODE="0666", SYMLINK+="hidVoltronic"' > /etc/udev﻿/rules.d/15-voltronic.rules
```

When the device is connected it will present in `/dev/hidVoltronic`.

Note that if multiple devices are to be connected to the same machine, an additional **udev** parameter should be specified such as the device serial number to with different symlink names

### Bluetooth
Newer generation [Axpert devices](http://voltronicpower.com/en-US/Product/Detail/Axpert-King-3KVA-5KVA) feature Bluetooth

No testing has been completed on these devices but Bluetooth simply operates exactly like RS232 and therefore there is no reason to believe it would be otherwise

### RS485
Newer generation [Axpert devices](http://voltronicpower.com/en-US/Product/Detail/Axpert-King-3KVA-5KVA) feature RS485 support

No testing has been completed on these devices but there is no reason to believe the underlying protocol has changed at all

## What does this library provide?
- Cross OS support
- Support for RS232, USB & Bluetooth (Perhaps RS485)
- CRC calculation written in C
- Common interface for all device communication methods

## What does this library NOT provide?
Specific commands as given above (as examples). ie `QPI`, etc.

The aim of this library is to act as a *driver* that allows interfacing with Voltronic devices through a simple API.
What a command does, and how the data the device returns should be parsed, is a higher level concern.

Much of the community is able to easily understand the commands, how to parse them and their effect.
The biggest barier to this author appears to be how to actually communicate with the device.

The communication concern requires a fair amount of knowledge of CRC, how to work with bytes and operating system API understanding ie> [termios](http://man7.org/linux/man-pages/man3/termios.3.html), [DCB](https://docs.microsoft.com/en-us/windows/desktop/api/winbase/ns-winbase-_dcb), tc.

Especially USB & USB HID is complex and has significantly complex APIs that are different across EVERY operating system. Meaning Windows, Linux, OSX and \*BSD (excl. OSX) all implement different APIs to communicate with HID devices. Even using a library like HIDAPI requires understanding the underlying fundamentals

## Dependencies
To remove a lot of the heavy lifting, the library relies:
- [libserialport](https://sigrok.org/wiki/Libserialport)
- [HIDAPI](https://github.com/signal11/hidapi)

## Building

### Dependencies

**Install depedencies:**
Each operating system will have a different list of prerequisites before the dependencies can be built.

See a more detailed list below 

**Build libserialport:**
```sh
git clone git://sigrok.org/libserialport lib/libserialport/
cd lib/libserialport/
./autogen.sh
./configure
make
make install # Requires sudo or run as su
```

**Build libhidapi:**
```sh
git clone https://github.com/signal11/hidapi.git lib/libhidapi/
cd lib/libhidapi/
./bootstrap
./configure
make
make install # Requires sudo or su
```

### FreeBSD

Tested on FreeBSD 10, 11, 12

Required dependencies:
```sh
su
pkg install gcc git autoconf automake libtool libiconv gmake
```

**gmake** instead of make to build

### Linux

Required dependencies:

**Ubuntu:**
```sh
sudo apt-get clean
sudo apt-get update
sudo apt-get install gcc git autoconf automake libtool pkg-config libudev-dev libusb-1.0-0-dev
```

**Amazon Linux:**
```sh
sudo yum clean all
sudo yum install gcc git autoconf automake libtool pkg-config libudev-devel libusb1-devel
```

**make hidraw** to build linking to HIDRaw **Recommended**
**make libusb** to build linking to libusb

### Windows

Unknown TBD

### OSX

The library was developed & tested on OSX High Sierra but the system is setup as a dev machine.
As such the complete list of dependencies have long since been forgotten.

At the very least:
- Homebrew
- gcc
- git

**make** to build
