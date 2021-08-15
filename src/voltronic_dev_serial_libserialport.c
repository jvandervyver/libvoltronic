#include "voltronic_dev_impl.h"
#include "voltronic_dev_serial.h"
#include <libserialport.h>

#define VOLTRONIC_DEV_SP(_impl_ptr_) ((struct sp_port*) (_impl_ptr_))

static int voltronic_dev_serial_configure(
  struct sp_port* port,
  const baud_rate_t baud_rate,
  const data_bits_t data_bits,
  const stop_bits_t stop_bits,
  const serial_parity_t parity
);

voltronic_dev_t voltronic_serial_create(
  const char* name,
  const baud_rate_t baud_rate,
  const data_bits_t data_bits,
  const stop_bits_t stop_bits,
  const serial_parity_t parity
) {
  enum sp_return sp_result;
  struct sp_port* port = 0;
  if (name != 0) {
    struct sp_port* tmp_port = { 0 };
    SET_LAST_ERROR(0);
    if ((sp_result = sp_get_port_by_name(name, &tmp_port)) == SP_OK) {
      port = tmp_port;
    }
  }

  if (port != 0) {
    SET_LAST_ERROR(0);
    if ((sp_result = sp_open(port, SP_MODE_READ_WRITE)) == SP_OK) {
      SET_LAST_ERROR(0);
      if (voltronic_dev_serial_configure(port, baud_rate, data_bits, stop_bits, parity) > 0) {
        sp_flush(port, SP_BUF_BOTH);
        SET_LAST_ERROR(0);
        return voltronic_dev_internal_create((void*) port);
      }
    }

    const last_error_t last_error = GET_LAST_ERROR();
    sp_free_port(port);
    SET_LAST_ERROR(last_error);
  }

  return 0;
}

inline int voltronic_dev_impl_read(
  void* impl_ptr,
  char* buffer,
  const size_t buffer_size,
  const unsigned int timeout_milliseconds
) {
  SET_LAST_ERROR(0);
  return (int) sp_blocking_read_next(
    VOLTRONIC_DEV_SP(impl_ptr),
    (void*) buffer,
    buffer_size,
    (unsigned int) timeout_milliseconds
  );
}

inline int voltronic_dev_impl_write(
  void* impl_ptr,
  const char* buffer,
  const size_t buffer_size,
  const unsigned int timeout_milliseconds
) {
  SET_LAST_ERROR(0);
  return (int) sp_blocking_write(
    VOLTRONIC_DEV_SP(impl_ptr),
    (const void*) buffer,
    buffer_size,
    (unsigned int) timeout_milliseconds
  );
}

inline int voltronic_dev_impl_close(void* impl_ptr) {
  struct sp_port* sp_port = VOLTRONIC_DEV_SP(impl_ptr);
  SET_LAST_ERROR(0);
  const enum sp_return result = sp_close(sp_port);
  if (result == SP_OK) {
    sp_free_port(sp_port);
    return 1;
  } else {
    return 0;
  }
}

static inline int voltronic_dev_baud_rate(
  const baud_rate_t baud_rate
) {
  return (int) baud_rate;
}

static inline int voltronic_dev_data_bits(
  const data_bits_t data_bits
) {
  switch(data_bits){
    case DATA_BITS_FIVE: return 5;
    case DATA_BITS_SIX: return 6;
    case DATA_BITS_SEVEN: return 7;
    case DATA_BITS_EIGHT: return 8;
    default: return -1;
  }
}

static inline int voltronic_dev_stop_bits(
  const stop_bits_t stop_bits
) {
  switch(stop_bits){
    case STOP_BITS_ONE: return 1;
    case STOP_BITS_TWO: return 2;
    case STOP_BITS_ONE_AND_ONE_HALF: return 3;
    default: return -1;
  }
}

static inline enum sp_parity voltronic_dev_serial_parity(
  const serial_parity_t parity
) {
  switch(parity){
    case SERIAL_PARITY_NONE: return SP_PARITY_NONE;
    case SERIAL_PARITY_ODD: return SP_PARITY_ODD;
    case SERIAL_PARITY_EVEN: return SP_PARITY_EVEN;
    case SERIAL_PARITY_MARK: return SP_PARITY_MARK;
    case SERIAL_PARITY_SPACE: return SP_PARITY_SPACE;
    default: return SP_PARITY_INVALID;
  }
}

static int voltronic_dev_serial_configure(
  struct sp_port* port,
  const baud_rate_t baud_rate,
  const data_bits_t data_bits,
  const stop_bits_t stop_bits,
  const serial_parity_t parity
) {
  const int sp_baud_rate = voltronic_dev_baud_rate(baud_rate);
  const int sp_data_bits = voltronic_dev_data_bits(data_bits);
  const int sp_stop_bits = voltronic_dev_stop_bits(stop_bits);
  const enum sp_parity sp_sp_parity = voltronic_dev_serial_parity(parity);

  if ((sp_data_bits == -1) ||
    (sp_data_bits == -1) ||
    (sp_stop_bits == -1) ||
    (sp_sp_parity == SP_PARITY_INVALID)) {

    SET_INVALID_INPUT();
    return 0;
  }

  int result = 0;
  struct sp_port_config *config;

  SET_LAST_ERROR(0);
  if (sp_new_config(&config) == SP_OK) {
    SET_LAST_ERROR(0);
    if (sp_get_config(port, config) == SP_OK) {
      SET_LAST_ERROR(0);
      if ((sp_set_config_baudrate (config, sp_baud_rate) == SP_OK) &&
        (sp_set_config_bits(config, sp_data_bits) == SP_OK) &&
        (sp_set_config_stopbits(config, sp_stop_bits) == SP_OK) &&
        (sp_set_config_parity(config, sp_sp_parity) == SP_OK)) {

        SET_LAST_ERROR(0);
        if (sp_set_config(port, config) == SP_OK) {
          result = 1;
        }
      }
    }

    const last_error_t last_error = GET_LAST_ERROR();
    sp_free_config(config);
    SET_LAST_ERROR(last_error);
  }

  return result;
}
