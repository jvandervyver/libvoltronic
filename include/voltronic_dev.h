#ifndef __VOLTRONIC__DEV__H__
#define __VOLTRONIC__DEV__H__

  #include <stddef.h>

  /**
  * Enable changing errno on errors
  */
  #define ENABLE_ERRNO 1

  typedef struct voltronic_dev_struct_t* voltronic_dev_t;

  int voltronic_dev_read(
    const voltronic_dev_t dev,
    char* buffer,
    const size_t buffer_size,
    const unsigned long timeout_milliseconds);

  int voltronic_dev_write(
    const voltronic_dev_t dev,
    const char* buffer,
    const size_t buffer_size);

  int voltronic_dev_execute(
    const voltronic_dev_t dev,
    const char* send_buffer,
    size_t send_buffer_length,
    char* receive_buffer,
    size_t receive_buffer_length,
    const unsigned long timeout_milliseconds);

  int voltronic_dev_close(voltronic_dev_t dev);

#endif
