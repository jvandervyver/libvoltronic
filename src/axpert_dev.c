#include "axpert_dev.h"
#include <stdlib.h>
#include <string.h>

struct axpert_dev_struct_t {
  void* impl_ptr;
  const axpert_dev_read_f read;
  const axpert_dev_write_f write;
  const axpert_dev_close_f close;
};

axpert_dev_t axpert_dev_create(
    void* impl_ptr,
    const axpert_dev_read_f read_function,
    const axpert_dev_write_f write_function,
    const axpert_dev_close_f close_function) {

  do {
    if (impl_ptr == 0) break;
    if (read_function == 0) break;
    if (write_function == 0) break;
    if (close_function == 0) break;

    const axpert_dev_t dev = malloc(sizeof(struct axpert_dev_struct_t));
    if (dev == 0) break;

    const struct axpert_dev_struct_t dev_struct = {
      impl_ptr,
      read_function,
      write_function,
      close_function };

    memcpy(dev, &dev_struct , sizeof(struct axpert_dev_struct_t));

    return dev;
  } while(0);

  return 0;
}

int axpert_dev_read(const axpert_dev_t dev, char* buffer, const size_t buffer_size) {
  if (dev != 0) {
    const axpert_dev_read_f read_function = dev->read;
    if (read_function != 0) {
      void* impl_ptr = dev->impl_ptr;
      if (impl_ptr != 0) {
        if (buffer != 0) {
          return read_function(impl_ptr, buffer, buffer_size);
        }
      }
    }
  }

  return -1;
}

int axpert_dev_write(const axpert_dev_t dev, const char* buffer, const size_t buffer_size) {
  if (dev != 0) {
    const axpert_dev_write_f write_function = dev->write;
    if (write_function != 0) {
      void* impl_ptr = dev->impl_ptr;
      if (impl_ptr != 0) {
        if (buffer != 0) {
          return write_function(impl_ptr, buffer, buffer_size);
        }
      }
    }
  }

  return -1;
}

int axpert_dev_close(axpert_dev_t dev) {
  if (dev != 0) {
    const axpert_dev_close_f close_function = dev->close;
    if (close_function != 0) {
      void* impl_ptr = dev->impl_ptr;
      if (impl_ptr != 0) {
        const int result = close_function(impl_ptr);
        if (result > 0) {
          dev->impl_ptr = 0;
          free(dev);
          return result;
        }
      }
    }
  }

  return -1;
}
