#ifndef __VOLTRONIC__DEV__H__
#define __VOLTRONIC__DEV__H__

  #include <stddef.h>

  typedef struct voltronic_dev_struct_t* voltronic_dev_t;

  int voltronic_dev_read(const voltronic_dev_t dev, char* buffer, const size_t buffer_size, const unsigned long timeout_milliseconds);
  int voltronic_dev_write(const voltronic_dev_t dev, const char* buffer, const size_t buffer_size);
  int voltronic_dev_close(voltronic_dev_t dev);

#endif
