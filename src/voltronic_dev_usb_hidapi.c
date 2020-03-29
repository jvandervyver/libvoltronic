#include "voltronic_dev_impl.h"
#include "voltronic_dev_usb.h"
#include "hidapi.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#if defined(_WIN32) || defined(WIN32)
  #include <Windows.h>
#else
  #include <stdint.h>
#endif

#define VOLTRONIC_DEV_USB(_impl_ptr_) ((hid_device*) (_impl_ptr_))
#define HID_REPORT_SIZE 8
#define GET_REPORT_SIZE(_val_) \
  (((_val_) > HID_REPORT_SIZE) ? HID_REPORT_SIZE : (_val_))

static int voltronic_dev_usb_read(
  void* impl_ptr,
  char* buffer,
  const size_t buffer_size,
  const unsigned int timeout_milliseconds);

static int voltronic_dev_usb_write(
  void* impl_ptr,
  const char* buffer,
  const size_t buffer_size,
  const unsigned int timeout_milliseconds);

static int voltronic_dev_usb_close(void* impl_ptr);

static inline void voltronic_usb_init_hidapi(void);

static inline size_t voltronic_usb_wchar_size(
  const char* buffer,
  size_t size);

voltronic_dev_t voltronic_usb_create(
    const unsigned int vendor_id,
    const unsigned int product_id,
    const char* serial_number) {

  if (is_platform_supported_by_libvoltronic()) {
    voltronic_usb_init_hidapi();

    void* impl_ptr = 0;
    if (serial_number == 0) {
      impl_ptr = hid_open(
        (unsigned short) vendor_id,
        (unsigned short) product_id,
        (const wchar_t*) 0);
    } else {
      const size_t length = voltronic_usb_wchar_size(
          serial_number, strlen(serial_number) + 1);

      if (length > 0) {
        wchar_t* wstring = ALLOCATE_MEMORY(sizeof(wchar_t) * (length + 1));
        wstring[length] = 0;

        mbstowcs(wstring, serial_number, length);
        impl_ptr = hid_open(
          (unsigned short) vendor_id,
          (unsigned short) product_id,
          (const wchar_t*) wstring);

        FREE_MEMORY(wstring);
      }
    }

    if (impl_ptr != 0) {
      const voltronic_dev_t result = voltronic_dev_create(
        impl_ptr,
        &voltronic_dev_usb_read,
        &voltronic_dev_usb_write,
        &voltronic_dev_usb_close);

      if (result != 0) {
        return result;
      }

      hid_close(VOLTRONIC_DEV_USB(impl_ptr));
    }
  }

  return 0;
}

static inline int voltronic_dev_usb_read(
    void* impl_ptr,
    char* buffer,
    const size_t buffer_size,
    const unsigned int timeout_milliseconds) {

  return hid_read_timeout(
    VOLTRONIC_DEV_USB(impl_ptr),
    (unsigned char*) buffer,
    GET_REPORT_SIZE(buffer_size),
    (int) timeout_milliseconds);
}

static inline int voltronic_dev_usb_write(
    void* impl_ptr,
    const char* buffer,
    const size_t buffer_size,
    unsigned int timeout_milliseconds) {

  ++timeout_milliseconds; // stop unused warnings
  const int write_size = GET_REPORT_SIZE(buffer_size);
  unsigned char write_buffer[HID_REPORT_SIZE + 1] = { 0 };
  COPY_MEMORY(&write_buffer[1], buffer, write_size);

  const int bytes_written = hid_write(
      VOLTRONIC_DEV_USB(impl_ptr),
      (const unsigned char*) write_buffer,
      (size_t) (HID_REPORT_SIZE + 1));

  return GET_REPORT_SIZE(bytes_written);
}

static int voltronic_dev_usb_close(void* impl_ptr) {
  hid_close(VOLTRONIC_DEV_USB(impl_ptr));
  return 1;
}

static inline size_t voltronic_usb_wchar_size(
    const char* buffer,
    size_t size) {

  mblen(NULL, 0);
  mbtowc(NULL, NULL, 0);

  size_t wide_chars = 0;
  while(size > 0) {
    const int length = mblen(buffer, size);
    if (length < 1) {
      return 0;
    }

    ++wide_chars;
    buffer += length;
    size -= length;
  }

  return wide_chars;
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
