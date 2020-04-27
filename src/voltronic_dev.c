#include "voltronic_dev.h"
#include "voltronic_dev_impl.h"
#include "voltronic_crc.h"
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(WIN32)

  #include "windows.h"

  typedef DWORD millisecond_timestamp_t;

#elif defined(__APPLE__)

  #include <mach/mach_time.h>

  typedef uint64_t millisecond_timestamp_t;

#elif defined(ARDUINO)

  #include <Arduino.h>

  typedef unsigned long millisecond_timestamp_t;

#else

  #include <time.h>
  #include <sys/time.h>
  #include <stdint.h>
  #include <unistd.h>

  typedef uint64_t millisecond_timestamp_t;

#endif

#define END_OF_INPUT '\r'
#define END_OF_INPUT_SIZE sizeof(char)
#define NON_DATA_SIZE (sizeof(voltronic_crc_t) + END_OF_INPUT_SIZE)

#define WRITE_CRC_ON_EXECUTE_VALUE   (1 << 1)
#define READ_CRC_ON_EXECUTE_VALUE    (1 << 2)
#define VERIFY_CRC_ON_EXECUTE_VALUE  (1 << 3)

#define DEFAULT_OPTIONS ( \
  WRITE_CRC_ON_EXECUTE_VALUE | \
  READ_CRC_ON_EXECUTE_VALUE | \
  VERIFY_CRC_ON_EXECUTE_VALUE)

#if defined(_WIN32) || defined(WIN32)

  #define SET_INVALID_INPUT() SET_LAST_ERROR(ERROR_INVALID_DATA)
  #define SET_TIMEOUT_REACHED() SET_LAST_ERROR(WAIT_TIMEOUT)
  #define SET_BUFFER_OVERFLOW() SET_LAST_ERROR(ERROR_INSUFFICIENT_BUFFER)
  #define SET_CRC_ERROR() SET_LAST_ERROR(ERROR_CRC)
  #define SYSTEM_NOT_SUPPORTED() SET_LAST_ERROR(ERROR_CALL_NOT_IMPLEMENTED)

#else

  #define SET_INVALID_INPUT() SET_LAST_ERROR(EINVAL)
  #define SET_TIMEOUT_REACHED() SET_LAST_ERROR(ETIMEDOUT)
  #define SET_BUFFER_OVERFLOW() SET_LAST_ERROR(ENOBUFS)
  #define SET_CRC_ERROR() SET_LAST_ERROR(EBADMSG)
  #define SYSTEM_NOT_SUPPORTED() SET_LAST_ERROR(ENOSYS)

#endif

static millisecond_timestamp_t get_millisecond_timestamp(void);

struct voltronic_dev_struct_t {
  void* impl_ptr;
  const voltronic_dev_read_f read;
  const voltronic_dev_write_f write;
  const voltronic_dev_close_f close;
  unsigned int options;
};

voltronic_dev_t voltronic_dev_create(
    void* impl_ptr,
    const voltronic_dev_read_f read_function,
    const voltronic_dev_write_f write_function,
    const voltronic_dev_close_f close_function) {

  if (is_platform_supported_by_libvoltronic()) {
    if ((impl_ptr != 0) &&
      (read_function != 0) &&
      (write_function != 0) &&
      (close_function != 0)) {

      voltronic_dev_t dev = (voltronic_dev_t) ALLOCATE_MEMORY(sizeof(struct voltronic_dev_struct_t));
      if (dev != 0) {
        const struct voltronic_dev_struct_t dev_struct = {
          impl_ptr,
          read_function,
          write_function,
          close_function,
          DEFAULT_OPTIONS };

        COPY_MEMORY(dev, &dev_struct, sizeof(struct voltronic_dev_struct_t));
      }

      return dev;
    } else {
      SET_INVALID_INPUT();
    }
  }

  return 0;
}

int voltronic_dev_read(
    const voltronic_dev_t dev,
    char* buffer,
    const size_t buffer_size,
    const unsigned int timeout_milliseconds) {

  if (dev != 0 && buffer != 0 && buffer_size > 0) {
    const voltronic_dev_read_f read_function = dev->read;
    void* impl_ptr = dev->impl_ptr;

    if (read_function != 0) {
      const int result = read_function(
        impl_ptr,
        buffer,
        buffer_size,
        timeout_milliseconds);

      return result >= 0 ? result : -1;
    }
  }

  SET_INVALID_INPUT();
  return -1;
}

int voltronic_dev_write(
    const voltronic_dev_t dev,
    const char* buffer,
    const size_t buffer_size,
    const unsigned int timeout_milliseconds) {

  if (dev != 0 && buffer != 0 && buffer_size > 0) {
    const voltronic_dev_write_f write_function = dev->write;
    void* impl_ptr = dev->impl_ptr;

    if (write_function != 0) {
      const int result = write_function(
        impl_ptr,
        buffer,
        buffer_size,
        timeout_milliseconds);

      return result >= 0 ? result : -1;
    }
  }

  SET_INVALID_INPUT();
  return -1;
}

static unsigned int get_voltronic_opt_mask(
  const voltronic_dev_opt_t opt) {

  switch(opt){
    case WRITE_CRC_ON_EXECUTE:
      return WRITE_CRC_ON_EXECUTE_VALUE;

    case READ_CRC_ON_EXECUTE:
      return READ_CRC_ON_EXECUTE_VALUE;

    case VERIFY_CRC_ON_EXECUTE:
      return VERIFY_CRC_ON_EXECUTE_VALUE;

    default:
      return 0;
  }
}

int is_voltronic_dev_opt_set(
  const voltronic_dev_t dev,
  const voltronic_dev_opt_t opt) {

  if (dev != 0) {
    const unsigned int value = get_voltronic_opt_mask(opt);
    if (value != 0) {
      if (dev->options & value) {
        return 1;
      } else {
        return 0;
      }
    }
  }

  SET_INVALID_INPUT();
  return -1;
}

int set_voltronic_dev_opt(
  const voltronic_dev_t dev,
  const voltronic_dev_opt_t opt) {

  if (dev != 0) {
    const unsigned int value = get_voltronic_opt_mask(opt);
    if (value != 0) {
      dev->options |= value;
      return 1;
    }
  }

  SET_INVALID_INPUT();
  return 0;
}

int unset_voltronic_dev_opt(
  const voltronic_dev_t dev,
  const voltronic_dev_opt_t opt) {

  if (dev != 0) {
    const unsigned int value = get_voltronic_opt_mask(opt);
    if (value != 0) {
      dev->options &= ~value;
      return 1;
    }
  }

  SET_INVALID_INPUT();
  return 0;
}

int voltronic_dev_close(voltronic_dev_t dev) {
  if (dev != 0) {
    const voltronic_dev_close_f close_function = dev->close;
    void* impl_ptr = dev->impl_ptr;

    if (close_function != 0 && impl_ptr != 0) {
      const int result = close_function(impl_ptr);
      if (result > 0) {
        dev->impl_ptr = 0;
        FREE_MEMORY(dev);
        return 1;
      }
    }
  }

  SET_INVALID_INPUT();
  return 0;
}

static int voltronic_read_data_loop(
    const voltronic_dev_t dev,
    char* buffer,
    size_t buffer_length,
    const unsigned int timeout_milliseconds) {

  unsigned int size = 0;

  const millisecond_timestamp_t start_time = get_millisecond_timestamp();
  millisecond_timestamp_t elapsed = 0;

  while(1) {
    int bytes_read = voltronic_dev_read(
      dev,
      buffer,
      buffer_length,
      timeout_milliseconds - elapsed);

    if (bytes_read >= 0) {
      while(bytes_read) {
        --bytes_read;
        ++size;

        if (*buffer == END_OF_INPUT) {
          return size;
        }

        buffer += sizeof(char);
        --buffer_length;
      }

      elapsed = get_millisecond_timestamp() - start_time;
      if (elapsed >= timeout_milliseconds) {
        SET_TIMEOUT_REACHED();
        return -1;
      }

      if (buffer_length <= 0) {
        SET_BUFFER_OVERFLOW();
        return -1;
      }
    } else {
      return bytes_read;
    }
  }
}

static int voltronic_receive_data(
    const voltronic_dev_t dev,
    char* buffer,
    const size_t buffer_length,
    const unsigned int timeout_milliseconds) {

  const int result = voltronic_read_data_loop(
    dev,
    buffer,
    buffer_length,
    timeout_milliseconds);

  if (result >= 0) {
    if (dev->options & READ_CRC_ON_EXECUTE_VALUE) {
      if (((size_t) result) >= NON_DATA_SIZE) {
        const size_t data_size = result - NON_DATA_SIZE;
        const voltronic_crc_t read_crc = read_voltronic_crc(&buffer[data_size], NON_DATA_SIZE);
        const voltronic_crc_t calculated_crc = calculate_voltronic_crc(buffer, data_size);
        buffer[data_size] = 0;

        if ((dev->options & VERIFY_CRC_ON_EXECUTE_VALUE) ||
            (read_crc == calculated_crc)) {

          return data_size;
        }
      }

      SET_CRC_ERROR();
    } else {
      if (((size_t) result) >= END_OF_INPUT_SIZE) {
        const size_t data_size = result - END_OF_INPUT_SIZE;
        buffer[data_size] = 0;
        return data_size;
      }
    }
  } else {
    return result;
  }

  return -1;
}

static int voltronic_write_data_loop(
    const voltronic_dev_t dev,
    const char* buffer,
    size_t buffer_length,
    const unsigned int timeout_milliseconds) {

  const millisecond_timestamp_t start_time = get_millisecond_timestamp();
  millisecond_timestamp_t elapsed = 0;

  int bytes_left = buffer_length;
  while(1) {
    const int write_result = voltronic_dev_write(dev, buffer, bytes_left, timeout_milliseconds);

    if (write_result >= 0) {
      bytes_left -= write_result;
      if (bytes_left > 0) {
        buffer = &buffer[write_result];
      } else {
        return buffer_length;
      }

      elapsed = get_millisecond_timestamp() - start_time;
      if (elapsed >= timeout_milliseconds) {
        SET_TIMEOUT_REACHED();
        return -1;
      }
    } else {
      return write_result;
    }
  }
}

static int voltronic_send_data(
    const voltronic_dev_t dev,
    const char* buffer,
    const size_t buffer_length,
    const unsigned int timeout_milliseconds) {

  size_t copy_length;
  char* copy;
  if (dev->options & WRITE_CRC_ON_EXECUTE_VALUE) {
    const voltronic_crc_t crc = calculate_voltronic_crc(buffer, buffer_length);

    copy_length = buffer_length + NON_DATA_SIZE;
    copy = (char*) ALLOCATE_MEMORY(copy_length * sizeof(char));

    write_voltronic_crc(crc, &copy[buffer_length], NON_DATA_SIZE);
  } else {
    copy_length = buffer_length + END_OF_INPUT_SIZE;
    copy = (char*) ALLOCATE_MEMORY(copy_length * sizeof(char));
  }

  COPY_MEMORY(copy, buffer, buffer_length * sizeof(char));
  copy[copy_length - 1] = END_OF_INPUT;

  const int result = voltronic_write_data_loop(
      dev,
      copy,
      copy_length,
      timeout_milliseconds);

  FREE_MEMORY(copy);

  return result;

}

int voltronic_dev_execute(
    const voltronic_dev_t dev,
    const char* send_buffer,
    size_t send_buffer_length,
    char* receive_buffer,
    size_t receive_buffer_length,
    const unsigned int timeout_milliseconds) {

  const millisecond_timestamp_t start_time = get_millisecond_timestamp();
  millisecond_timestamp_t elapsed = 0;
  int result;

  result = voltronic_send_data(
    dev,
    send_buffer,
    send_buffer_length,
    timeout_milliseconds);

  if (result > 0) {
    elapsed = get_millisecond_timestamp() - start_time;
    if (elapsed < timeout_milliseconds) {
      result = voltronic_receive_data(
        dev,
        receive_buffer,
        receive_buffer_length,
        timeout_milliseconds - elapsed);

      if (result > 0) {
        return result;
      }
    } else {
      SET_TIMEOUT_REACHED();
    }
  }

  return 0;
}

#if defined(_WIN32) || defined(WIN32)

  static millisecond_timestamp_t get_millisecond_timestamp(void) {
    return (millisecond_timestamp_t) GetTickCount();
  }

#elif defined(__APPLE__)

  static millisecond_timestamp_t get_millisecond_timestamp(void) {
    return (millisecond_timestamp_t) mach_absolute_time();
  }

#elif defined(ARDUINO)

  static millisecond_timestamp_t get_millisecond_timestamp(void) {
    return (millisecond_timestamp_t) millis();
  }

#else

  static millisecond_timestamp_t get_millisecond_timestamp(void) {
    millisecond_timestamp_t milliseconds = 0;

    #if defined(CLOCK_MONOTONIC)

      static int monotonic_clock_error = 0;
      if (monotonic_clock_error == 0) {
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
          milliseconds = (millisecond_timestamp_t) ts.tv_sec;
          milliseconds *= 1000;
          milliseconds += (millisecond_timestamp_t) (ts.tv_nsec / 1000000);
          return milliseconds;
        } else {
          monotonic_clock_error = 1;
        }
      }

    #endif

    struct timeval tv;
    if (gettimeofday(&tv, 0) == 0) {
      milliseconds = (millisecond_timestamp_t) tv.tv_sec;
      milliseconds *= 1000;
      milliseconds += (millisecond_timestamp_t) (tv.tv_usec / 1000);
    }

    return milliseconds;
  }

#endif

int is_platform_supported_by_libvoltronic(void) {
  /**
  * Operating system/cpu architecture validations
  * If any of these fail, things don't behave the code expects
  */
  if ((sizeof(char) == sizeof(unsigned char)) &&
    (sizeof(unsigned char) == 1) &&
    (sizeof(int) >= 2) &&
    (sizeof(unsigned int) >= 2) &&
    (sizeof(voltronic_crc_t) == 2) &&
    (sizeof(millisecond_timestamp_t) >= 4)) {

    return 1;
  } else {
    SYSTEM_NOT_SUPPORTED();
    return 0;
  }
}
