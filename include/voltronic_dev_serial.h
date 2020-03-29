#ifndef __VOLTRONIC__DEV__SERIAL__H__
#define __VOLTRONIC__DEV__SERIAL__H__

  #include "voltronic_dev.h"

  /**
   * The serial port baud rate configuration
   */
  typedef unsigned int baud_rate_t;

  /**
   * The serial port data bits configuration
   */
  typedef enum {
    DATA_BITS_FIVE,
    DATA_BITS_SIX,
    DATA_BITS_SEVEN,
    DATA_BITS_EIGHT
  } data_bits_t;

  /**
   * The serial port stop bits configuration
   */
  typedef enum {
    STOP_BITS_ONE,
    STOP_BITS_ONE_AND_ONE_HALF,
    STOP_BITS_TWO
  } stop_bits_t;

  /**
   * The serial port parity configuration
   */
  typedef enum {
    SERIAL_PARITY_NONE,
    SERIAL_PARITY_ODD,
    SERIAL_PARITY_EVEN,
    SERIAL_PARITY_MARK,
    SERIAL_PARITY_SPACE
  } serial_parity_t;

  /**
   * Create an opaque pointer to a voltronic device connected over serial
   *
   * name - The device name, ie. COM1; /dev/usb.serial; etc.
   * baud_rate - Baud rate configuration, ie. 2400
   * data_bits - Data bits configuration, ie. DATA_BITS_EIGHT
   * stop_bits - Stop bits configuration, ie. STOP_BITS_ONE
   * parity - Parity configuration, ie. SERIAL_PARITY_NONE
   *
   * Returns an opaque pointer to a voltronic device or 0 if an error occurred
   *
   * Function sets errno (POSIX)/LastError (Windows) to approriate error on failure 
   */
  voltronic_dev_t voltronic_serial_create(
    const char* name,
    const baud_rate_t baud_rate,
    const data_bits_t data_bits,
    const stop_bits_t stop_bits,
    const serial_parity_t parity);

#endif
