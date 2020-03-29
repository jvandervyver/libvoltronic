#include "voltronic_dev_impl.h"
#include "voltronic_dev_serial.h"
#include <libserialport.h>
#include <stdlib.h>
#include <string.h>

#define VOLTRONIC_DEV_SP(_impl_ptr_) ((struct sp_port*) (_impl_ptr_))

static int voltronic_dev_serial_read(
  void* impl_ptr,
  char* buffer,
  const size_t buffer_size,
  const unsigned long timeout_milliseconds);

static int voltronic_dev_serial_write(
  void* impl_ptr,
  const char* buffer,
  const size_t buffer_size);

static int voltronic_dev_serial_close(void* impl_ptr);

static int voltronic_dev_serial_configure(
    void* impl_ptr,
    const baud_rate_t baud_rate,
    const data_bits_t data_bits,
    const stop_bits_t stop_bits,
    const serial_parity_t parity);

voltronic_dev_t voltronic_serial_create(
    const char* name,
    const baud_rate_t baud_rate,
    const data_bits_t data_bits,
    const stop_bits_t stop_bits,
    const serial_parity_t parity) {

  void* impl_ptr = 0;
  if (name != 0) {
    struct sp_port *port_ptr;
    const enum sp_return result = sp_get_port_by_name(name, &port_ptr);
    if (result == SP_OK) {
      impl_ptr = (void*) port_ptr;
    }
  }

  if (impl_ptr != 0) {
    const enum sp_return open_status = sp_open(VOLTRONIC_DEV_SP(impl_ptr), SP_MODE_READ_WRITE);
    if (open_status == SP_OK) {
      if (voltronic_dev_serial_configure(impl_ptr, baud_rate, data_bits, stop_bits, parity) > 0) {
        return voltronic_dev_create(
          impl_ptr,
          &voltronic_dev_serial_read,&voltronic_dev_serial_write,
          &voltronic_dev_serial_close);
      }
    }

    sp_free_port(VOLTRONIC_DEV_SP(impl_ptr));

    return 0;
  } else {
    return 0;
  }
}

static inline int voltronic_dev_serial_read(
    void* impl_ptr,
    char* buffer,
    const size_t buffer_size,
    const unsigned long timeout_milliseconds) {

  return sp_blocking_read_next(
    VOLTRONIC_DEV_SP(impl_ptr),
    buffer,
    buffer_size,
    timeout_milliseconds);
}

static inline int voltronic_dev_serial_write(
    void* impl_ptr,
    const char* buffer,
    const size_t buffer_size) {

  return sp_nonblocking_write(
    VOLTRONIC_DEV_SP(impl_ptr),
    buffer,
    buffer_size);
}

static int voltronic_dev_serial_close(void* impl_ptr) {
  const enum sp_return result = sp_close(VOLTRONIC_DEV_SP(impl_ptr));
  if (result == SP_OK) {
    sp_free_port(VOLTRONIC_DEV_SP(impl_ptr));
    return 1;
  } else {
    return -1;
  }
}

static inline int voltronic_dev_baud_rate(const baud_rate_t baud_rate) {
  return (int) baud_rate;
}

static inline int voltronic_dev_data_bits(const data_bits_t data_bits) {
  switch(data_bits){
    case DATA_BITS_FIVE: return 5;
    case DATA_BITS_SIX: return 6;
    case DATA_BITS_SEVEN: return 7;
    case DATA_BITS_EIGHT: return 8;
  }

  return -1;
}

static inline int voltronic_dev_stop_bits(const stop_bits_t stop_bits) {
  switch(stop_bits){
    case STOP_BITS_ONE: return 1;
    case STOP_BITS_ONE_AND_ONE_HALF: return 3;
    case STOP_BITS_TWO: return 2;
  }

  return -1;
}

static inline enum sp_parity voltronic_dev_serial_parity(
    const serial_parity_t parity) {

  switch(parity){
    case SERIAL_PARITY_NONE: return SP_PARITY_NONE;
    case SERIAL_PARITY_ODD: return SP_PARITY_ODD;
    case SERIAL_PARITY_EVEN: return SP_PARITY_EVEN;
    case SERIAL_PARITY_MARK: return SP_PARITY_MARK;
    case SERIAL_PARITY_SPACE: return SP_PARITY_SPACE;
  }

  return SP_PARITY_INVALID;
}

static int voltronic_dev_serial_configure(
    void* impl_ptr,
    const baud_rate_t baud_rate,
    const data_bits_t data_bits,
    const stop_bits_t stop_bits,
    const serial_parity_t parity) {

  struct sp_port_config *config_ptr;
  if (sp_new_config(&config_ptr) == SP_OK) {
    if (sp_get_config(VOLTRONIC_DEV_SP(impl_ptr), config_ptr) == SP_OK) {
      if (sp_set_config_baudrate (config_ptr, voltronic_dev_baud_rate(baud_rate)) == SP_OK) {
        if (sp_set_config_bits(config_ptr, voltronic_dev_data_bits(data_bits)) == SP_OK) {
          if (sp_set_config_stopbits(config_ptr, voltronic_dev_stop_bits(stop_bits)) == SP_OK) {
            if (sp_set_config_parity(config_ptr, voltronic_dev_serial_parity(parity)) == SP_OK) {
              if (sp_set_config(VOLTRONIC_DEV_SP(impl_ptr), config_ptr) == SP_OK) {
                return 1;
              }
            }
          }
        }
      }
    }

    sp_free_config(config_ptr);
  }

  return -1;
}
