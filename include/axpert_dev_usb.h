#ifndef __AXPERT__DEV__USB__H__
#define __AXPERT__DEV__USB__H__

  #include "axpert_dev.h"

  axpert_dev_t axpert_usb_create(
    const unsigned int vendor_id,
    const unsigned int product_id,
    const char* serial_number);

  //int axpert_usb_close(axpert_dev_t dev);

#endif
