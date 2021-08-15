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

  #include "voltronic_dev.h"

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
  int voltronic_dev_impl_read(
    void* impl_ptr,
    char* buffer,
    const size_t buffer_size,
    const unsigned int timeout_milliseconds
  );

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
  int voltronic_dev_impl_write(
    void* impl_ptr,
    const char* buffer,
    const size_t buffer_size,
    const unsigned int timeout_milliseconds
  );

  /**
   * Accept the implementation pointer and close the underlying device connection
   *
   * Returns 0 on failure, anything else is considered success
   *
   * On failure set the appropriate error using SET_LAST_ERROR
   */
  int voltronic_dev_impl_close(
    void* impl_ptr
  );

  /**
   * Create the opaque pointer representing a connection to a physical voltronic device
   *
   * impl_ptr -> The underlying implementation's device pointer
   *
   * On failure sets the appropriate error using SET_LAST_ERROR
   */
  voltronic_dev_t voltronic_dev_internal_create(
    void* impl_ptr
  );

  /**
   * May change if operating system requires it.
   */
  #define ALLOCATE_MEMORY(__size__) \
    malloc(((size_t) (__size__)))

  #define COPY_MEMORY(__destination__, __source__, __size__) \
    memcpy(((void*) (__destination__)), \
      ((const void*) (__source__)), \
      ((size_t) (__size__)))

  #define FREE_MEMORY(__ptr__) \
    free(((void*) (__ptr__)))

  #if defined(_WIN32) || defined(WIN32)
    #include "windows.h"

    typedef DWORD last_error_t;

    #define SET_LAST_ERROR(_last_error_value_)  SetLastError((_last_error_value_))
    #define GET_LAST_ERROR()                    GetLastError()
    #define SET_INVALID_INPUT()                 SET_LAST_ERROR(ERROR_INVALID_DATA)

  #else

    #include <errno.h>

    typedef int last_error_t;

    #define SET_LAST_ERROR(_errno__value_)      errno = (_errno__value_)
    #define GET_LAST_ERROR()                    (errno)
    #define SET_INVALID_INPUT()                 SET_LAST_ERROR(EINVAL)

  #endif

#endif
