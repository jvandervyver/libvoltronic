#ifndef __VOLTRONIC__DEV__SERIAL__H__
#define __VOLTRONIC__DEV__SERIAL__H__

  #include "voltronic_dev.h"

  typedef unsigned int baud_rate_t;

  typedef enum {
    DATA_BITS_FIVE,
    DATA_BITS_SIX,
    DATA_BITS_SEVEN,
    DATA_BITS_EIGHT,
    DATA_BITS_NINE
  } data_bits_t;

  typedef enum {
    STOP_BITS_ONE,
    STOP_BITS_ONE_AND_ONE_HALF,
    STOP_BITS_TWO
  } stop_bits_t;

  typedef enum {
    SERIAL_PARITY_NONE,
    SERIAL_PARITY_ODD,
    SERIAL_PARITY_EVEN,
    SERIAL_PARITY_MARK,
    SERIAL_PARITY_SPACE
  } serial_parity_t;

  voltronic_dev_t voltronic_serial_create(
    const char* name,
    const baud_rate_t baud_rate,
    const data_bits_t data_bits,
    const stop_bits_t stop_bits,
    const serial_parity_t parity);

#endif
