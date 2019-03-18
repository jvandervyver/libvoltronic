#include "axpert_dev_serial.h"
#include <libserialport.h>
#include <stdlib.h>
#include <string.h>

#define AXPERT_DEV_SP(_impl_ptr_) ((struct sp_port*) (_impl_ptr_))

static int axpert_dev_serial_free(char** serial_ports, int size);
static int axpert_dev_serial_read(void* impl_ptr, char* buffer, const size_t buffer_size);
static int axpert_dev_serial_write(void* impl_ptr, const char* buffer, const size_t buffer_size);
static int axpert_dev_serial_close(void* impl_ptr);
static int axpert_dev_serial_configure(
    void* impl_ptr,
    const baud_rate_t baud_rate,
    const data_bits_t data_bits,
    const stop_bits_t stop_bits,
    const serial_parity_t parity);

axpert_dev_t axpert_serial_create(
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

  do {
    if (impl_ptr == 0) break;

    const enum sp_return open_status = sp_open(AXPERT_DEV_SP(impl_ptr), SP_MODE_READ_WRITE);
    if (open_status != SP_OK) break;

    if (axpert_dev_serial_configure(
      impl_ptr, baud_rate, data_bits, stop_bits, parity) <= 0) break;

    return axpert_dev_create(
      impl_ptr,
      &axpert_dev_serial_read,
      &axpert_dev_serial_write,
      &axpert_dev_serial_close);
  } while(0);
  sp_free_port(AXPERT_DEV_SP(impl_ptr));

  return 0;
}

int axpert_serial_free(char** list) {
  return axpert_dev_serial_free(list, -1);
}

char** axpert_serial_list() {
  struct sp_port **serial_ports;

  const enum sp_return list_ports_result = sp_list_ports(&serial_ports);
  unsigned int serial_ports_size = 0;
  if (list_ports_result == SP_OK) {
    for(; serial_ports[serial_ports_size]; ++serial_ports_size) {
    }
  } else {
    return 0;
  }

  if (serial_ports_size <= 0) {
    sp_free_port_list(serial_ports);
    return 0;
  }

  char** result = malloc(sizeof(char*) * (serial_ports_size + 1));
  result[serial_ports_size] = 0;

  for(unsigned int index = 0; index < serial_ports_size; ++index) {
    const char* serial_port_name = sp_get_port_name(serial_ports[index]);
    if (serial_port_name == 0) {
      axpert_dev_serial_free(result, serial_ports_size);
      sp_free_port_list(serial_ports);
      return 0;
    }

    const size_t length = strlen(serial_port_name);
    if (length > 0) {
      char* copy = (result[index] = malloc(sizeof(char) * (length + 1)));
      memcpy(copy, serial_port_name, length);
      copy[length] = 0;
    } else {
      result[index] = malloc(sizeof(char));
      result[index] = 0;
    }
  }

  sp_free_port_list(serial_ports);
  return result;
}

static int axpert_dev_serial_read(void* impl_ptr, char* buffer, const size_t buffer_size) {
  if (impl_ptr != 0) {
    return sp_nonblocking_read(AXPERT_DEV_SP(impl_ptr), buffer, buffer_size);
  } else {
    return -1;
  }
}

static int axpert_dev_serial_write(void* impl_ptr, const char* buffer, const size_t buffer_size) {
  if (impl_ptr != 0) {
    return sp_nonblocking_write(AXPERT_DEV_SP(impl_ptr), buffer, buffer_size);
  } else {
    return -1;
  }
}

static int axpert_dev_serial_close(void* impl_ptr) {
  if (impl_ptr != 0) {
    const enum sp_return result = sp_close(AXPERT_DEV_SP(impl_ptr));
    if (result == SP_OK) {
      sp_free_port(AXPERT_DEV_SP(impl_ptr));
      return 1;
    }
  }

  return -1;
}

static inline int axpert_dev_baud_rate(const baud_rate_t baud_rate) {
  return (int) baud_rate;
}

static inline int axpert_dev_data_bits(const data_bits_t data_bits) {
  switch(data_bits){
    case DATA_BITS_FIVE: return 5;
    case DATA_BITS_SIX: return 6;
    case DATA_BITS_SEVEN: return 7;
    case DATA_BITS_EIGHT: return 8;
    case DATA_BITS_NINE: return 9;
  }

  return -1;
}

static inline int axpert_dev_stop_bits(const stop_bits_t stop_bits) {
  switch(stop_bits){
    case STOP_BITS_ONE: return 1;
    case STOP_BITS_ONE_AND_ONE_HALF: return 3;
    case STOP_BITS_TWO: return 2;
  }

  return -1;
}

static inline enum sp_parity axpert_dev_serial_parity(const serial_parity_t parity) {
  switch(parity){
    case SERIAL_PARITY_NONE: return SP_PARITY_NONE;
    case SERIAL_PARITY_ODD: return SP_PARITY_ODD;
    case SERIAL_PARITY_EVEN: return SP_PARITY_EVEN;
    case SERIAL_PARITY_MARK: return SP_PARITY_MARK;
    case SERIAL_PARITY_SPACE: return SP_PARITY_SPACE;
  }

  return SP_PARITY_INVALID;
}

static int axpert_dev_serial_configure(
    void* impl_ptr,
    const baud_rate_t baud_rate,
    const data_bits_t data_bits,
    const stop_bits_t stop_bits,
    const serial_parity_t parity) {

  struct sp_port_config *config_ptr;

  if (sp_new_config(&config_ptr) != SP_OK) {
    return -1;
  }

  int result = -1;
  do {
    if (sp_get_config(AXPERT_DEV_SP(impl_ptr), config_ptr) != SP_OK) break;

    if (sp_set_config_baudrate (config_ptr, axpert_dev_baud_rate(baud_rate)) != SP_OK) break;
    if (sp_set_config_bits(config_ptr, axpert_dev_data_bits(data_bits)) != SP_OK) break;
    if (sp_set_config_stopbits(config_ptr, axpert_dev_stop_bits(stop_bits)) != SP_OK) break;
    if (sp_set_config_parity(config_ptr, axpert_dev_serial_parity(parity)) != SP_OK) break;

    if (sp_set_config(AXPERT_DEV_SP(impl_ptr), config_ptr) != SP_OK) break;
    result = 1;
  } while(0);

  sp_free_config(config_ptr);
  return result;
}

static int axpert_dev_serial_free(char** serial_ports, int size) {
  if (serial_ports != 0) {
    int index = -1;
    char* cstring;

    if (size > 0) {
      while(--size) {
        cstring = serial_ports[++index];
        if (cstring != 0) {
          free(cstring);
        }
      }
    } else {
      while((cstring = serial_ports[++index])) {
        free(cstring);
      }      
    }

    free(serial_ports);
  }

  return 1;
}
