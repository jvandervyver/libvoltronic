#include "axpert_dev_usb.h"
#include "hidapi.h"
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(WIN32)
  #include <Windows.h>
#else
  #include <stdint.h>
#endif

#define AXPERT_DEV_USB(_impl_ptr_) ((hid_device*) (_impl_ptr_))

static inline void axpert_usb_init_hidapi();
static int axpert_dev_usb_read(void* impl_ptr, char* buffer, const size_t buffer_size, const unsigned long timeout_milliseconds);
static int axpert_dev_usb_write(void* impl_ptr, const char* buffer, const size_t buffer_size);
static int axpert_dev_usb_close(void* impl_ptr);

axpert_dev_t axpert_usb_create(
    const unsigned int vendor_id,
    const unsigned int product_id,
    const char* serial_number) {

  axpert_usb_init_hidapi();

  hid_device* hid_device = hid_open(vendor_id, product_id, 0);
  if (hid_device != 0) {
    const axpert_dev_t result = axpert_dev_create(
      (void*) hid_device,
      &axpert_dev_usb_read,
      &axpert_dev_usb_write,
      &axpert_dev_usb_close);

    if (result != 0) {
      return result;
    }

    hid_close(hid_device);
  }

  return 0;
}

static int axpert_dev_usb_read(void* impl_ptr, char* buffer, const size_t buffer_size, const unsigned long timeout_milliseconds) {
  if (impl_ptr != 0) {
    return hid_read_timeout(AXPERT_DEV_USB(impl_ptr), (unsigned char*) buffer, buffer_size, timeout_milliseconds);
  } else {
    return -1;
  }
}

static int axpert_dev_usb_write(void* impl_ptr, const char* buffer, const size_t buffer_size) {
  if (impl_ptr != 0) {
    return hid_write(AXPERT_DEV_USB(impl_ptr), (const unsigned char*) buffer, buffer_size);
  } else {
    return -1;
  }
}

static int axpert_dev_usb_close(void* impl_ptr) {
  if (impl_ptr != 0) {
    hid_close(AXPERT_DEV_USB(impl_ptr));
    return 1;
  }

  return -1;
}

static inline void axpert_usb_exit_hidapi() {
  hid_exit();
}

static inline void axpert_usb_init_hidapi() {
  #if defined(_WIN32) || defined(WIN32)
    static unsigned __int8 hidapi_init_complete = 0;
  #else
    static uint_fast8_t hidapi_init_complete = 0;
  #endif

  if (hidapi_init_complete == 0) {
    if (hid_init() == 0) {
      atexit(axpert_usb_exit_hidapi);
      hidapi_init_complete = 1;
    }
  }
}
