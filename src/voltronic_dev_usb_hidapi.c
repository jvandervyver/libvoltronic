#include <string.h>
#include <stdlib.h>
#include "voltronic_dev_impl.h"
#include "voltronic_dev_usb.h"
#include "hidapi.h"

#define HID_REPORT_SIZE 8

#define VOLTRONIC_DEV_USB(_impl_ptr_) \
  ((hid_device*) (_impl_ptr_))

#define GET_REPORT_SIZE(_val_) \
  (((_val_) > HID_REPORT_SIZE) ? HID_REPORT_SIZE : (_val_))

static inline void voltronic_usb_init_hidapi(void);

voltronic_dev_t voltronic_usb_create(
  const unsigned int vendor_id,
  const unsigned int product_id) {

  voltronic_usb_init_hidapi();

  SET_LAST_ERROR(0);
  hid_device* dev = hid_open(
    (unsigned short) vendor_id,
    (unsigned short) product_id,
    (const wchar_t*) 0);

  if (dev != 0) {
    SET_LAST_ERROR(0);
    return voltronic_dev_internal_create((void*) dev);
  }

  return 0;
}

inline int voltronic_dev_impl_read(
  void* impl_ptr,
  char* buffer,
  const size_t buffer_size,
  const unsigned int timeout_milliseconds) {

  SET_LAST_ERROR(0);
  return hid_read_timeout(
    VOLTRONIC_DEV_USB(impl_ptr),
    (unsigned char*) buffer,
    GET_REPORT_SIZE(buffer_size),
    (int) timeout_milliseconds);
}

inline int voltronic_dev_impl_write(
  void* impl_ptr,
  const char* buffer,
  const size_t buffer_size,
  unsigned int timeout_milliseconds) {

  ++timeout_milliseconds; // stop unused warnings
  const int write_size = GET_REPORT_SIZE(buffer_size);
  unsigned char write_buffer[HID_REPORT_SIZE + 1] = { 0 };
  COPY_MEMORY(&write_buffer[1], buffer, write_size);

  SET_LAST_ERROR(0);
  const int bytes_written = hid_write(
    VOLTRONIC_DEV_USB(impl_ptr),
    (const unsigned char*) write_buffer,
    (size_t) (HID_REPORT_SIZE + 1));

  return GET_REPORT_SIZE(bytes_written);
}

inline int voltronic_dev_impl_close(void* impl_ptr) {
  hid_close(VOLTRONIC_DEV_USB(impl_ptr));
  return 1;
}

static inline void voltronic_usb_exit_hidapi(void) {
  hid_exit();
}

static inline void voltronic_usb_init_hidapi(void) {
  static int hidapi_init_complete = 0;

  if (hidapi_init_complete == 0) {
    if (hid_init() == 0) {
      atexit(voltronic_usb_exit_hidapi);
      hidapi_init_complete = 1;
    }
  }
}
