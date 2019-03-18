#ifndef __AXPERT__DEV__H__
#define __AXPERT__DEV__H__

  #include <stddef.h>

  typedef struct axpert_dev_struct_t* axpert_dev_t;

  int axpert_dev_read(const axpert_dev_t dev, char* buffer, const size_t buffer_size);
  int axpert_dev_write(const axpert_dev_t dev, const char* buffer, const size_t buffer_size);
  int axpert_dev_close(axpert_dev_t dev);

  /**
   * Used internally by implementations
   */

  typedef int (*axpert_dev_read_f)(void* impl_ptr, char* buffer, const size_t buffer_size);
  typedef int (*axpert_dev_write_f)(void* impl_ptr, const char* buffer, const size_t buffer_size);
  typedef int (*axpert_dev_close_f)(void* impl_ptr);

  axpert_dev_t axpert_dev_create(
      void* impl_ptr,
      const axpert_dev_read_f read_function,
      const axpert_dev_write_f write_function,
      const axpert_dev_close_f close_function);

#endif
