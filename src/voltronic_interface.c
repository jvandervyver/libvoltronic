#include "voltronic_interface.h"
#include "voltronic_crc.h"
#include "time_util.h"
#include <string.h>
#include <stdlib.h>

static const char END_OF_INPUT = '\r';

#define SIZE_OF_CRC_CR (sizeof(voltronic_crc_t) + sizeof(END_OF_INPUT))

static int voltronic_read_data_loop(
    voltronic_dev_t dev,
    char* buffer,
    size_t buffer_length,
    const unsigned long timeout_milliseconds) {

  unsigned int size = 0;
  unsigned int parse_complete = 1;

  const millisecond_timestamp_t start_time = get_millisecond_timestamp();
  millisecond_timestamp_t elapsed = 0;

  while(1) {
    int read_result = voltronic_dev_read(dev, buffer, buffer_length, timeout_milliseconds - elapsed);
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
    voltronic_dev_t dev,
    char* buffer,
    const size_t buffer_length,
    const unsigned long timeout_milliseconds) {

  int result = voltronic_read_data_loop(dev, buffer, buffer_length, timeout_milliseconds);
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
    voltronic_dev_t dev,
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
    voltronic_dev_t dev,
    const char* buffer,
    size_t buffer_length,
    const unsigned long timeout_milliseconds) {

  const voltronic_crc_t crc = calculate_voltronic_crc(buffer, buffer_length);

  const size_t copy_length = buffer_length + SIZE_OF_CRC_CR;
  char* copy = malloc(copy_length * sizeof(char));
  memcpy(copy, buffer, buffer_length * sizeof(char));

  write_voltronic_crc(crc, &copy[buffer_length], SIZE_OF_CRC_CR);
  copy[copy_length - 1] = END_OF_INPUT;

  int result = voltronic_write_data_loop(dev, copy, copy_length, timeout_milliseconds);
  free(copy);

  return result;
}

int voltronic_execute_command(
    voltronic_dev_t dev,
    const char* command,
    size_t command_length,
    char* buffer,
    size_t buffer_length,
    const unsigned long timeout_milliseconds) {

  const millisecond_timestamp_t start_time = get_millisecond_timestamp();
  millisecond_timestamp_t elapsed = 0;

  int result = 0;
  do {
    result = voltronic_send_data(dev, command, command_length, timeout_milliseconds);
    elapsed = get_millisecond_timestamp() - start_time;

    if (result <= 0) {
      break;
    }

    if (elapsed >= timeout_milliseconds) {
      return -1;
    }

    result = voltronic_receive_data(dev, buffer, buffer_length, timeout_milliseconds - elapsed);
  } while(0);

  return result;
}
