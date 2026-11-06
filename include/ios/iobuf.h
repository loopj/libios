#pragma once

#include <stdint.h>

/** A network packet buffer from the kernel's pool. */
struct iobuf {
  /** The next buffer of a chained packet. */
  struct iobuf *next;

  /** The previous buffer of a chained packet. */
  struct iobuf *prev;

  /** A word the kernel leaves alone. */
  uint32_t reserved;

  /** The start of the buffer. */
  uint8_t *buffer;

  /** The start of the data, inside the buffer. */
  uint8_t *data;

  /** The buffer's size in bytes. */
  uint16_t buffer_size;

  /** The data's length in bytes. */
  uint16_t data_len;

  /** The kernel's own fields, such as the reference count. */
  uint8_t kernel[0x10];
};
