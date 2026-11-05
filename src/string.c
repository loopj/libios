// Partial implementation of libc's string.h, MEM1/MEM2 aware

#include <stdint.h>
#include <string.h>

// First address past MEM1, where a byte store is used as is
#define MEM1_END 0x01800000

// Store one byte, merged into its word below MEM1_END, like the stock modules
static void store_byte(uint8_t *at, uint8_t value)
{
  // Store the byte as is above MEM1
  if ((uintptr_t)at >= MEM1_END) {
    *at = value;
    return;
  }

  // Merge the byte into its big endian word and store the word
  uint32_t *word = (uint32_t *)((uintptr_t)at & ~3u);
  uint32_t shift = (3 - ((uintptr_t)at & 3)) * 8;
  *word          = (*word & ~(0xffu << shift)) | ((uint32_t)value << shift);
}

// Copy len bytes from source to destination, whole words between the edges
void *memcpy(void *destination, const void *source, size_t len)
{
  uint8_t *to         = destination;
  const uint8_t *from = source;

  // Store bytes up to a word boundary of the destination
  while (len && ((uintptr_t)to & 3)) {
    store_byte(to++, *from++);
    len--;
  }

  // Move whole words, loaded as words when the source is aligned too and gathered from bytes otherwise
  if (((uintptr_t)from & 3) == 0) {
    while (len >= 4) {
      *(uint32_t *)to = *(const uint32_t *)from;
      to += 4;
      from += 4;
      len -= 4;
    }
  } else {
    while (len >= 4) {
      *(uint32_t *)to = ((uint32_t)from[0] << 24) | ((uint32_t)from[1] << 16) | ((uint32_t)from[2] << 8) | from[3];
      to += 4;
      from += 4;
      len -= 4;
    }
  }

  // Store the tail bytes
  while (len--)
    store_byte(to++, *from++);

  return destination;
}

// Fill len bytes of destination with value, whole words between the edges
void *memset(void *destination, int value, size_t len)
{
  uint8_t *to  = destination;
  uint8_t byte = (uint8_t)value;

  // Store bytes up to a word boundary
  while (len && ((uintptr_t)to & 3)) {
    store_byte(to++, byte);
    len--;
  }

  // Store whole words
  uint32_t word = byte * 0x01010101u;
  while (len >= 4) {
    *(uint32_t *)to = word;
    to += 4;
    len -= 4;
  }

  // Store the tail bytes
  while (len--)
    store_byte(to++, byte);

  return destination;
}

// Compare len bytes of two buffers, returning zero when they match
int memcmp(const void *left, const void *right, size_t len)
{
  const uint8_t *left_bytes  = left;
  const uint8_t *right_bytes = right;

  // Order by the first byte that differs
  for (size_t i = 0; i < len; i++) {
    if (left_bytes[i] != right_bytes[i])
      return left_bytes[i] - right_bytes[i];
  }

  return 0;
}

// Count the bytes of text before its terminator
size_t strlen(const char *text)
{
  // Walk to the terminator
  size_t len = 0;
  while (text[len])
    len++;

  return len;
}

// Count the bytes of text before its terminator, at most max
size_t strnlen(const char *text, size_t max)
{
  // Walk to the terminator or max
  size_t len = 0;
  while (len < max && text[len])
    len++;

  return len;
}

// Compare two strings, returning zero when they match
int strcmp(const char *left, const char *right)
{
  // Walk both while they match and left has not ended
  while (*left && *left == *right) {
    left++;
    right++;
  }

  // Order by the first byte that differs
  return (uint8_t)*left - (uint8_t)*right;
}
