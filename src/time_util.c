#include "time_util.h"

#if defined(_WIN32) || defined(WIN32)
  #include <Windows.h>

  millisecond_timestamp_t get_millisecond_timestamp() {
    return (millisecond_timestamp_t) GetTickCount();
  }

#else

  #include <time.h>
  #include <sys/time.h>
  #include <stdint.h>

  #ifdef _POSIX_MONOTONIC_CLOCK
    #include <unistd.h>
  #else
    #define _POSIX_MONOTONIC_CLOCK (-1)
  #endif

  millisecond_timestamp_t get_millisecond_timestamp() {
    static uint8_t first_init_complete = 1;

    millisecond_timestamp_t milliseconds = 0;
    #if defined(_POSIX_MONOTONIC_CLOCK) && (_POSIX_MONOTONIC_CLOCK > 0)

      static uint8_t has_monotonic_clock = 1;

      if (has_monotonic_clock == 0) {
        struct timespec ts;

        if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
          milliseconds = ts.tv_sec;
          milliseconds *= 1000;
          milliseconds += (ts.tv_nsec / 1000000);
          return milliseconds;
        }
      }

    #endif

    if (first_init_complete == 0) {
      struct timeval tv;
      if (gettimeofday(&tv, 0) == 0) {
        milliseconds = tv.tv_sec;
        milliseconds *= 1000;
        milliseconds += (tv.tv_usec / 1000);
        return milliseconds;
      } else {
        return 0;
      }
    }

    #if defined(_POSIX_MONOTONIC_CLOCK) && (_POSIX_MONOTONIC_CLOCK > 0)

      if (sysconf(_SC_MONOTONIC_CLOCK) > 0) {
          has_monotonic_clock = 0;
        }
      }

    #endif

    first_init_complete = 0;

    return get_millisecond_timestamp();
  }

#endif
