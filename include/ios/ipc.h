#pragma once

#include <stdint.h>

/** Request kinds, in struct ipcmessage's command. */
enum ios_command {
  IOS_OPEN   = 1,
  IOS_CLOSE  = 2,
  IOS_READ   = 3,
  IOS_WRITE  = 4,
  IOS_SEEK   = 5,
  IOS_IOCTL  = 6,
  IOS_IOCTLV = 7,
};

/** One buffer of an ioctlv. */
struct ioctlv {
  /** The buffer. */
  void *data;

  /** Its length in bytes. */
  uint32_t len;
};

/** A request the kernel hands a resource manager, and the reply an asynchronous call gets back. */
struct ipcmessage {
  /** The request's kind, an enum ios_command. */
  uint32_t command;

  /** The result, written by the reply. */
  int32_t result;

  /** The value the resource manager answered the open with. */
  int32_t fd;

  /** The arguments of each request kind. */
  union {
    /** An open's arguments. */
    struct {
      /** The path opened. */
      const char *path;

      /** The open mode. */
      uint32_t mode;

      /** The process that opened it. */
      uint32_t process_id;

      /** The opener's group. */
      uint16_t gid;
    } open;

    /** A read's or write's arguments. */
    struct {
      /** The buffer. */
      void *data;

      /** Its length in bytes. */
      uint32_t len;
    } rw;

    /** A seek's arguments. */
    struct {
      /** The offset. */
      int32_t offset;

      /** Where the offset counts from. */
      int32_t origin;
    } seek;

    /** An ioctl's arguments. */
    struct {
      /** The request number. */
      uint32_t request;

      /** The input buffer. */
      void *in;

      /** The input's length in bytes. */
      uint32_t in_len;

      /** The output buffer. */
      void *out;

      /** The output's length in bytes. */
      uint32_t out_len;
    } ioctl;

    /** An ioctlv's arguments. */
    struct {
      /** The request number. */
      uint32_t request;

      /** The number of input vectors, first in the array. */
      uint32_t in_count;

      /** The number of output vectors, after the inputs. */
      uint32_t out_count;

      /** The vectors. */
      struct ioctlv *vector;
    } ioctlv;
  };
};
