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

static int voltronic_dev_usb_read(void* impl_ptr, char* buffer, const size_t buffer_size, const unsigned long timeout_milliseconds);
static int voltronic_dev_usb_write(void* impl_ptr, const char* buffer, const size_t buffer_size);
static int voltronic_dev_usb_close(void* impl_ptr);

static inline void voltronic_usb_init_hidapi();
static inline size_t voltronic_usb_wchar_size(const char* buffer, size_t size);

voltronic_dev_t voltronic_usb_create(
    const unsigned int vendor_id,
    const unsigned int product_id,
    const char* serial_number) {

  voltronic_usb_init_hidapi();

  void* impl_ptr = 0;
  if (serial_number == 0) {
    impl_ptr = hid_open(vendor_id, product_id, 0);
  } else {
    const size_t length = voltronic_usb_wchar_size(serial_number, strlen(serial_number) + 1);
    if (length > 0) {
      wchar_t* wstring = malloc(sizeof(wchar_t) * (length + 1));
      wstring[length] = 0;

      mbstowcs(wstring, serial_number, length);
      impl_ptr = hid_open(vendor_id, product_id, wstring);

      free(wstring);
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

  return 0;
}

static int voltronic_dev_usb_read(void* impl_ptr, char* buffer, const size_t buffer_size, const unsigned long timeout_milliseconds) {
  if (impl_ptr != 0) {
    return hid_read_timeout(VOLTRONIC_DEV_USB(impl_ptr), (unsigned char*) buffer, buffer_size, timeout_milliseconds);
  } else {
    return -1;
  }
}

static int voltronic_dev_usb_write(void* impl_ptr, const char* buffer, const size_t buffer_size) {
  if (impl_ptr != 0) {
    return hid_write(VOLTRONIC_DEV_USB(impl_ptr), (const unsigned char*) buffer, buffer_size);
  } else {
    return -1;
  }
}

static int voltronic_dev_usb_close(void* impl_ptr) {
  if (impl_ptr != 0) {
    hid_close(VOLTRONIC_DEV_USB(impl_ptr));
    return 1;
  }

  return -1;
}

static inline size_t voltronic_usb_wchar_size(const char* buffer, size_t size) {
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

static inline void voltronic_usb_exit_hidapi() {
  hid_exit();
}

static inline void voltronic_usb_init_hidapi() {
  #if defined(_WIN32) || defined(WIN32)
    static unsigned __int8 hidapi_init_complete = 0;
  #else
    static uint_fast8_t hidapi_init_complete = 0;
  #endif

  if (hidapi_init_complete == 0) {
    if (hid_init() == 0) {
      atexit(voltronic_usb_exit_hidapi);
      hidapi_init_complete = 1;
    }
  }
}
