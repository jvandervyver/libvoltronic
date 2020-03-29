#include "voltronic_dev_serial.h"

/**
* This class is only here to link everything related to serial port to assert compilation
*/

int main() {
  // Create a serial port dev
  voltronic_dev_t dev = voltronic_serial_create(
    "invalid_name", 2400, DATA_BITS_EIGHT, STOP_BITS_ONE, SERIAL_PARITY_NONE);

  if (dev == 0) {
    return 1;
  }

  // Query the device
  char buffer[256];
  int result = voltronic_dev_execute(dev, "QPI", 3, buffer, sizeof(buffer), 1000);

  // Close the connection to the device
  voltronic_dev_close(dev);

  return result <= 0 ? 2 : 0;
}
