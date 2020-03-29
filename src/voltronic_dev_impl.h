#ifndef __VOLTRONIC__DEV__IMPL__H__
#define __VOLTRONIC__DEV__IMPL__H__

  /**
  * ------------------------------------------------------------------
   * ------------------------------------------------------------------
   * Used internally by implementations of voltronic_dev.h
   * Don't include unless you are building an implementation
   * ------------------------------------------------------------------
   * ------------------------------------------------------------------
   */


  /*
  * Write error states to operating specific error handler
  */
  #if defined(_WIN32) || defined(WIN32)
    #include <Windows.h>

    #define SET_LAST_ERROR(__val__) SetLastError((__val__));
  #else
    #include <errno.h>

    #define SET_LAST_ERROR(__val__) errno = (__val__);
  #endif

  #include "voltronic_dev.h"

  /**
  * It is recommended you validate
  * if (is_platform_supported_by_libvoltronic()) { ... }
  * Before attempting to create a dev
  */
  int is_platform_supported_by_libvoltronic(void);

  /**
   * Read up to buffer_size bytes from the device
   *
   * impl_ptr -> The underlying implementation's device pointer
   * buffer -> The buffer to store data from the device
   * buffer_size -> The maximum number of bytes to read
   * timeout_milliseconds -> Number of milliseconds before giving up
   *
   * Return the number of bytes successfully read from the device.
   * On failure returns < 0
   *
   * On failure set the appropriate error using SET_LAST_ERROR
   */
  typedef int (*voltronic_dev_read_f)(
      void* impl_ptr,
      char* buffer,
      const size_t buffer_size,
      const unsigned int timeout_milliseconds);

  /**
   * Write the provided buffer data to the device
   *
   * impl_ptr -> The underlying implementation's device pointer
   * buffer -> The data to write to the device
   * buffer_size -> Number of bytes to write to the device
   * timeout_milliseconds -> Number of milliseconds before giving up
   *
   * Return the number of bytes successfully written to the device.
   * On failure returns < 0
   *
   * On failure set the appropriate error using SET_LAST_ERROR
   */
  typedef int (*voltronic_dev_write_f)(
      void* impl_ptr,
      const char* buffer,
      const size_t buffer_size,
      const unsigned int timeout_milliseconds);

  /**
   * Accept the implementation pointer and close the underlying device connection
   *
   * Returns 0 on failure, anything else is considered success
   *
   * On failure set the appropriate error using SET_LAST_ERROR
   */
  typedef int (*voltronic_dev_close_f)(
      void* impl_ptr);

  /**
   * Create the opaque pointer representing a connection to a physical voltronic device
   *
   * impl_ptr -> The underlying implementation's device pointer
   * read_function -> Function to read from the physical device
   * write_function -> Function to write to the physical device
   * close_function -> Function to close the connection to the underlying device
   *
   * On failure sets the appropriate error using SET_LAST_ERROR
   */ 
  voltronic_dev_t voltronic_dev_create(
      void* impl_ptr,
      const voltronic_dev_read_f read_function,
      const voltronic_dev_write_f write_function,
      const voltronic_dev_close_f close_function);

  /**
   * May change if operating system requires it.
   */
  #define ALLOCATE_MEMORY(__size__) \
    malloc(((size_t) (__size__)))

  #define COPY_MEMORY(__destination__, __source__, __size__) \
    memcpy(((void*) (__destination__)), ((const void*) (__source__)), ((size_t) (__size__)))

  #define FREE_MEMORY(__ptr__) \
    free(((void*) (__ptr__)))

#endif
