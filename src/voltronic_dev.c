#include "voltronic_dev.h"
#include "voltronic_dev_impl.h"
#include "voltronic_crc.h"
#include "time_util.h"
#include <string.h>
#include <stdlib.h>

static const char END_OF_INPUT = '\r';

#define SIZE_OF_CRC_CR (sizeof(voltronic_crc_t) + sizeof(END_OF_INPUT))

struct voltronic_dev_struct_t {
  void* impl_ptr;
  const voltronic_dev_read_f read;
  const voltronic_dev_write_f write;
  const voltronic_dev_close_f close;
};

voltronic_dev_t voltronic_dev_create(
    void* impl_ptr,
    const voltronic_dev_read_f read_function,
    const voltronic_dev_write_f write_function,
    const voltronic_dev_close_f close_function) {

  do {
    if (impl_ptr == 0) break;
    if (read_function == 0) break;
    if (write_function == 0) break;
    if (close_function == 0) break;

    const voltronic_dev_t dev = malloc(sizeof(struct voltronic_dev_struct_t));
    if (dev == 0) break;

    const struct voltronic_dev_struct_t dev_struct = {
      impl_ptr,
      read_function,
      write_function,
      close_function };

    memcpy(dev, &dev_struct , sizeof(struct voltronic_dev_struct_t));

    return dev;
  } while(0);

  return 0;
}

int voltronic_dev_read(
    const voltronic_dev_t dev,
    char* buffer,
    const size_t buffer_size,
    const unsigned long timeout_milliseconds) {

  if (dev != 0) {
    const voltronic_dev_read_f read_function = dev->read;
    if (read_function != 0) {
      void* impl_ptr = dev->impl_ptr;
      if ((impl_ptr != 0) && (buffer != 0) && (buffer_size > 0)) {
        return read_function(impl_ptr, buffer, buffer_size, timeout_milliseconds);
      }
    }
  }

  return -1;
}

int voltronic_dev_write(
    const voltronic_dev_t dev,
    const char* buffer,
    const size_t buffer_size) {

  if (dev != 0) {
    const voltronic_dev_write_f write_function = dev->write;
    if (write_function != 0) {
      void* impl_ptr = dev->impl_ptr;
      if ((impl_ptr != 0) && (buffer != 0) && (buffer_size > 0)) {
        return write_function(impl_ptr, buffer, buffer_size);
      }
    }
  }

  return -1;
}

int voltronic_dev_close(voltronic_dev_t dev) {
  if (dev != 0) {
    const voltronic_dev_close_f close_function = dev->close;
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

static int voltronic_read_data_loop(
    const voltronic_dev_t dev,
    char* buffer,
    size_t buffer_length,
    const unsigned long timeout_milliseconds) {

  unsigned int size = 0;
  unsigned int parse_complete = 1;

  const millisecond_timestamp_t start_time = get_millisecond_timestamp();
  millisecond_timestamp_t elapsed = 0;

  while(1) {
    int read_result = voltronic_dev_read(
      dev,
      buffer,
      buffer_length,
      timeout_milliseconds - elapsed);

    elapsed = get_millisecond_timestamp() - start_time;

    if (read_result < 0) {
      return read_result;
    }

    while(read_result) {
      --read_result;
      ++size;

      if (*buffer == END_OF_INPUT) {
        parse_complete = 0;
        read_result = 0;
      }

      ++buffer;
      --buffer_length;
    }

    if (parse_complete == 0) {
      return size;
    }

    if (elapsed >= timeout_milliseconds) {
      return -1;
    }

    if (buffer_length <= 0) {
      return -1;
    }
  }
}

static int voltronic_receive_data(
    const voltronic_dev_t dev,
    char* buffer,
    const size_t buffer_length,
    const unsigned long timeout_milliseconds) {

  int result = voltronic_read_data_loop(
    dev,
    buffer,
    buffer_length,
    timeout_milliseconds);

  if (result <= 0) {
    return result;
  }

  if (((size_t) result) < SIZE_OF_CRC_CR) {
    // Result couldn't possible contain the CRC & CR character
    return -1;
  }

  const size_t data_size = result - SIZE_OF_CRC_CR;
  const voltronic_crc_t read_crc = read_voltronic_crc(&buffer[data_size], SIZE_OF_CRC_CR);
  const voltronic_crc_t calculated_crc = calculate_voltronic_crc(buffer, data_size);
  buffer[data_size] = 0;

  if (read_crc != calculated_crc) {
    // CRC Mismatch
    return -1;
  }

  return data_size;
}

static int voltronic_write_data_loop(
    const voltronic_dev_t dev,
    const char* buffer,
    size_t buffer_length,
    const unsigned long timeout_milliseconds) {

  const millisecond_timestamp_t start_time = get_millisecond_timestamp();
  millisecond_timestamp_t elapsed = 0;

  while(1) {
    int write_result = voltronic_dev_write(dev, buffer, buffer_length);
    elapsed = get_millisecond_timestamp() - start_time;

    if (write_result < 0) {
      return write_result;
    }

    if (write_result > 0) {
      buffer_length -= write_result;
      if (buffer_length > 0) {
        buffer = &buffer[write_result];
      } else {
        return 1;
      }
    }

    if (elapsed >= timeout_milliseconds) {
      return -1;
    }
  }
}

static int voltronic_send_data(
    const voltronic_dev_t dev,
    const char* buffer,
    size_t buffer_length,
    const unsigned long timeout_milliseconds) {

  const voltronic_crc_t crc = calculate_voltronic_crc(buffer, buffer_length);

  const size_t copy_length = buffer_length + SIZE_OF_CRC_CR;
  char* copy = malloc(copy_length * sizeof(char));
  memcpy(copy, buffer, buffer_length * sizeof(char));

  write_voltronic_crc(crc, &copy[buffer_length], SIZE_OF_CRC_CR);
  copy[copy_length - 1] = END_OF_INPUT;

  int result = voltronic_write_data_loop(
    dev,
    copy,
    copy_length,
    timeout_milliseconds);

  free(copy);

  return result;
}

int voltronic_dev_execute(
    const voltronic_dev_t dev,
    const char* send_buffer,
    size_t send_buffer_length,
    char* receive_buffer,
    size_t receive_buffer_length,
    const unsigned long timeout_milliseconds) {

  const millisecond_timestamp_t start_time = get_millisecond_timestamp();
  millisecond_timestamp_t elapsed = 0;

  int result = 0;
  do {
    result = voltronic_send_data(
      dev,
      send_buffer,
      send_buffer_length,
      timeout_milliseconds);

    elapsed = get_millisecond_timestamp() - start_time;

    if (result <= 0) {
      break;
    }

    if (elapsed >= timeout_milliseconds) {
      return -1;
    }

    result = voltronic_receive_data(
      dev,
      receive_buffer,
      receive_buffer_length,
      timeout_milliseconds - elapsed);

  } while(0);

  return result;
}
