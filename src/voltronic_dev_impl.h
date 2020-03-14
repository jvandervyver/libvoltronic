#ifndef __VOLTRONIC__DEV__IMPL__H__
#define __VOLTRONIC__DEV__IMPL__H__

  /**
   * Used internally by implementations of voltronic_dev.h
   * Don't include unless you are building an implementation
   */

  #include "voltronic_dev.h"

  typedef int (*voltronic_dev_read_f)(
      void* impl_ptr,
      char* buffer,
      const size_t buffer_size,
      const unsigned long timeout_milliseconds);

  typedef int (*voltronic_dev_write_f)(
      void* impl_ptr,
      const char* buffer,
      const size_t buffer_size);

  typedef int (*voltronic_dev_close_f)(
      void* impl_ptr);

  voltronic_dev_t voltronic_dev_create(
      void* impl_ptr,
      const voltronic_dev_read_f read_function,
      const voltronic_dev_write_f write_function,
      const voltronic_dev_close_f close_function);

  /*
  * Write error states to ERRNO
  */
  #define ENABLE_ERRNO 1

  #if defined(ENABLE_ERRNO) &&  ENABLE_ERRNO > 0
    #include <errno.h>
    #define SET_ERRNO(__val__) errno = __val__;
  #else
    #define SET_ERRNO(__val__) 0;
  #endif

#endif
