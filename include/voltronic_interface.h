#ifndef __VOLTRONIC__INTERFACE__H__
#define __VOLTRONIC__INTERFACE__H__

  #include <stddef.h>
  #include "voltronic_dev.h"

  int voltronic_execute_command(
    voltronic_dev_t dev,
    const char* command,
    size_t command_length,
    char* buffer,
    size_t buffer_length,
    const unsigned long timeout_milliseconds);

#endif
