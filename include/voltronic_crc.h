#ifndef __VOLTRONIC__CRC__H__
#define __VOLTRONIC__CRC__H__

  #include <stddef.h>

  /**
   * The underlying numeric type used to store the CRC
   */
  #if defined(_WIN32) || defined(WIN32)
    #include <Windows.h>

    typedef unsigned __int16 voltronic_crc_t;
  #else
    #include <stdint.h>

    typedef uint16_t voltronic_crc_t;
  #endif

  /**
   * Write a voltronic_crc_t to a buffer
   *
   * crc - CRC to write
   * buffer - Buffer to write CRC to
   * buffer_length - Size of the buffer
   *
   * Returns number of bytes written or 0 if the buffer has insufficient size
   */
  int write_voltronic_crc(
    const voltronic_crc_t crc,
    char* buffer,
    const size_t buffer_length);

  /**
   * Read a voltronic_crc_t from a buffer
   *
   * buffer - Buffer to read CRC from
   * buffer_length - Size of the buffer
   *
   * Returns the CRC read from the device or 0 if the device is of insufficient size
   */
  voltronic_crc_t read_voltronic_crc(
    const char* buffer,
    const size_t buffer_length);

  /**
   * Calculate the Voltronic CRC by reading a buffer and calculating the CRC from the bytes
   *
   * buffer - Buffer to read from
   * buffer_length - Number of bytes in the buffer
   *
   * Returns the CRC or 0 if the input is invalid
   */
  voltronic_crc_t calculate_voltronic_crc(
    const char* buffer,
    size_t buffer_length);

#endif
