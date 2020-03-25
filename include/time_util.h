#ifndef __TIME__UTIL__H__
#define __TIME__UTIL__H__

  #if defined(_WIN32) || defined(WIN32)
    #include <Windows.h>

    typedef DWORD voltronic_crc_t;
  #else
    #include <stdint.h>

    #ifdef UINT64_MAX
      typedef uint64_t millisecond_timestamp_t;
    #elif UINT32_MAX
      typedef uint32_t millisecond_timestamp_t;
    #endif
  #endif

  millisecond_timestamp_t get_millisecond_timestamp(void);

#endif
