#ifndef __AXPERT__CRC__H__
#define __AXPERT__CRC__H__

  #include <stddef.h>

  #if defined(_WIN32) || defined(WIN32)
    #include <Windows.h>

    typedef unsigned __int16 axpert_crc_t;
  #else
    #include <stdint.h>

    typedef uint_fast16_t axpert_crc_t;
  #endif

  /**
   * Switch to bit-by-bit CRC16 calculation algorithm instead of a table driven algorithm
   *
   * This will save 4KiB of program space at the cost of computation time
   */
  #define AXPERT_CRC_USE_TABLE_METHOD    1

  axpert_crc_t calculate_axpert_crc(const char* buffer, size_t buffer_length);

#endif
