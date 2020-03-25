#include "time_util.h"

#if defined(_WIN32) || defined(WIN32)

  #include <Windows.h>

  millisecond_timestamp_t get_millisecond_timestamp(void) {
    return (millisecond_timestamp_t) GetTickCount();
  }

#else

  #include <time.h>
  #include <sys/time.h>
  #include <stdint.h>
  #include <unistd.h>

  millisecond_timestamp_t get_millisecond_timestamp(void) {
    #if defined(CLOCK_MONOTONIC)
      static int monotonic_clock_error = 0;
    #endif

    millisecond_timestamp_t milliseconds = 0;
    #if defined(CLOCK_MONOTONIC)

      if (monotonic_clock_error == 0) {
        struct timespec ts;

        if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
          milliseconds = ts.tv_sec;
          milliseconds *= 1000;
          milliseconds += (ts.tv_nsec / 1000000);
          return milliseconds;
        } else {
          monotonic_clock_error = 1;
        }
      }

    #endif

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

#endif
