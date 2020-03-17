#ifndef __VOLTRONIC__CRC__H__
#define __VOLTRONIC__CRC__H__

  #include <stddef.h>

  #if defined(_WIN32) || defined(WIN32)
    #include <Windows.h>

    typedef unsigned __int16 voltronic_crc_t;
  #else
    #include <stdint.h>

    typedef uint16_t voltronic_crc_t;
  #endif

  int is_platform_supported(void);

  int write_voltronic_crc(
    const voltronic_crc_t crc,
    char* buffer,
    const size_t buffer_length);

  voltronic_crc_t read_voltronic_crc(
    const char* buffer,
    const size_t buffer_length);

  voltronic_crc_t calculate_voltronic_crc(
    const char* buffer,
    size_t buffer_length);

#endif
