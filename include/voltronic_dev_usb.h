#ifndef __VOLTRONIC__DEV__USB__H__
#define __VOLTRONIC__DEV__USB__H__

  #include "voltronic_dev.h"

  /**
   * Create an opaque pointer to a voltronic device connected over USB
   *
   * vendor_id - Device vendor id to search for. ie. 0x0665
   * product_id - Device product id to search for. ie. 0x5161
   *
   * Returns an opaque pointer to a voltronic device or 0 if an error occurred
   *
   * Function sets errno (POSIX)/LastError (Windows) to approriate error on failure 
   */
  voltronic_dev_t voltronic_usb_create(
    const unsigned int vendor_id,
    const unsigned int product_id);

#endif
