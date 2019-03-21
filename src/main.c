#include "voltronic_crc.h"
#include "voltronic_dev.h"
#include "voltronic_dev_serial.h"
#include "voltronic_dev_usb.h"
#include "voltronic_interface.h"
#include "time_util.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static const char command[] = "QPIGS";

int main() {

  //voltronic_dev_t dev = voltronic_serial_create("/dev/tty.usbserial", 2400, DATA_BITS_EIGHT, STOP_BITS_ONE, SERIAL_PARITY_NONE);
  voltronic_dev_t dev = voltronic_usb_create(0x0665, 0x5161, 0);
  if (dev == 0) {
    printf("Could not open /dev/usbserial\n");
    return 1;
  }

  char buffer[128];
  memset(buffer, 0, sizeof(buffer));
  char* buffer_ptr = buffer;
  int result = voltronic_execute_command(dev, command, strlen(command), buffer, sizeof(buffer), 1000);
  if (result > 0) {
    printf("Success on command %s, got %s\n", command, buffer);
  } else {
    printf("Failed to execute %s\n", command);
  }

  voltronic_dev_close(dev);
}
