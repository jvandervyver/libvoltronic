#ifndef __TIME__UTIL__H__
#define __TIME__UTIL__H__

  #if defined(_WIN32) || defined(WIN32)
    #include <Windows.h>

    typedef DWORD voltronic_crc_t;
  #elif defined(ARDUINO)
    typedef unsigned long millisecond_timestamp_t;
  #else
    #include <stdint.h>

    typedef uint64_t millisecond_timestamp_t;
  #endif

  /**
  * Get a millisecond timestamp.  This timestamp is not equivalent to a unix timestamp
  * and may in fact differ based on underlying operating system.  The only guarantee is
  * that it can be used to determine elapsed milliseconds between successive calls.
  *
  * The return type millisecond_timestamp_t is an operating system dependent integer type
  */
  millisecond_timestamp_t get_millisecond_timestamp(void);

#endif
