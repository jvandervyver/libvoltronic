#ifndef __VOLTRONIC__DEV__USB__H__
#define __VOLTRONIC__DEV__USB__H__

  #include "voltronic_dev.h"

  voltronic_dev_t voltronic_usb_create(
    const unsigned int vendor_id,
    const unsigned int product_id,
    const char* serial_number);

#endif
