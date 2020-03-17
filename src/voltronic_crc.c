#include "voltronic_crc.h"

#define IS_EQUAL(_ch_a_, _ch_b_) \
  ((_ch_a_) == (_ch_b_))

#define IS_RESERVED_BYTE(_ch_) \
  (IS_EQUAL(_ch_, 0x28) || \
   IS_EQUAL(_ch_, 0x0D) || \
   IS_EQUAL(_ch_, 0x0A))

#define CRC_SIZE \
  (sizeof(voltronic_crc_t))

#define IS_TYPE_COMPATIBLE() \
  ((sizeof(char) == sizeof(unsigned char)) && \
   (sizeof(unsigned char) == 1) && \
   (CRC_SIZE == 2))

#define VOLTRONIC_LOW_BYTE_POSITION 0
#define VOLTRONIC_HIGH_BYTE_POSITION 0

inline int is_platform_supported(void) {
  return IS_TYPE_COMPATIBLE() ? 1 : 0;
}

int write_voltronic_crc(
    const voltronic_crc_t crc,
    char* cstring_buffer,
    const size_t buffer_length) {

  if (IS_TYPE_COMPATIBLE() && buffer_length >= CRC_SIZE) {
    unsigned char* buffer = (unsigned char*) cstring_buffer;

    buffer[0] = (crc >> 8) & 0xFF;
    buffer[1] = crc & 0xFF;

    return CRC_SIZE;
  } else {
    return 0;
  }
}

voltronic_crc_t read_voltronic_crc(
    const char* cstring_buffer,
    const size_t buffer_length) {

  voltronic_crc_t crc = 0;
  if (IS_TYPE_COMPATIBLE() && buffer_length >= CRC_SIZE) {
    const unsigned char* buffer = (const unsigned char*) cstring_buffer;

    crc |= (voltronic_crc_t) buffer[0] << 8;
    crc |= (voltronic_crc_t) buffer[1];
  }

  return crc;
}

voltronic_crc_t calculate_voltronic_crc(
    const char* cstring_buffer,
    size_t buffer_length) {

  voltronic_crc_t crc = 0;

  if (IS_TYPE_COMPATIBLE() && buffer_length > 0) {

    static const voltronic_crc_t crc_table[16] = {
      0x0000, 0x1021, 0x2042, 0x3063,
      0x4084, 0x50A5, 0x60C6, 0x70E7,
      0x8108, 0x9129, 0xA14A, 0xB16B,
      0xC18C, 0xD1AD, 0xE1CE, 0xF1EF
    };

    const unsigned char* buffer = (const unsigned char*) cstring_buffer;
    unsigned char byte;
    do {
      byte = *buffer;

      crc = crc_table[(crc >> 12) ^ (byte >> 4)] ^ (crc << 4);
      crc = crc_table[(crc >> 12) ^ (byte & 0x0F)] ^ (crc << 4);

      buffer += sizeof(unsigned char);
    } while(--buffer_length);

    byte = crc;
    if (IS_RESERVED_BYTE(byte)) {
      crc += 1;
    }

    byte = crc >> 8;
    if (IS_RESERVED_BYTE(byte)) {
      crc += 1 << 8;
    }
  }

  return crc;
}
