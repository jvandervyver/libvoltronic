#include "axpert_dev_usb.h"
#include "hidapi.h"
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(WIN32)
  #include <Windows.h>
#else
  #include <stdint.h>
#endif

typedef enum {
  HIDAPI_ENSURE_INIT,
  HIDAPI_ENSURE_EXIT
} hidapi_library_state_t;

#define AXPERT_DEV_USB(_impl_ptr_) ((hid_device*) (_impl_ptr_))

static int axpert_dev_usb_read(void* impl_ptr, char* buffer, const size_t buffer_size, const unsigned long timeout_milliseconds);
static int axpert_dev_usb_write(void* impl_ptr, const char* buffer, const size_t buffer_size);
static int axpert_dev_usb_close(void* impl_ptr);
static inline void hidapi_bootstrap_state();

axpert_dev_t axpert_usb_create(
    const unsigned int vendor_id,
    const unsigned int product_id,
    const char* serial_number) {

  hidapi_bootstrap_state(HIDAPI_ENSURE_INIT);

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

static inline void hidapi_bootstrap_state(hidapi_library_state_t desired_state) {
  #if defined(_WIN32) || defined(WIN32)
    static unsigned __int8 hidapi_init_state_set = 0;
  #else
    static uint_fast8_t hidapi_init_state_set = 0;
  #endif

  if (hidapi_init_state_set == 0) {
    if (desired_state == HIDAPI_ENSURE_INIT) {
      if (hid_init() == 0) {
        hidapi_init_state_set = 1;
      }
    }
  } else {
    if (desired_state == HIDAPI_ENSURE_EXIT) {
      if (hid_exit() == 0) {
        hidapi_init_state_set = 0;
      }
    }
  }
}
