#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "iobuf.h"
#include "ipc.h"

/** Results the kernel returns, negative on failure. */
enum ios_error {
  /** Success. */
  IOS_ERROR_OK = 0,

  /** The caller may not do this, such as another process's event. */
  IOS_ERROR_ACCESS = -1,

  /** An argument is out of range, such as a thread priority above the caller's. */
  IOS_ERROR_INVALID = -4,

  /** A fixed table is full, such as all 100 threads of the system. */
  IOS_ERROR_MAX = -5,

  /** No device answers to the path, or the device refused the open. */
  IOS_ERROR_NOT_FOUND = -6,

  /** The message queue is full. */
  IOS_ERROR_QUEUE_FULL = -8,

  /** The kernel heap is out of memory. */
  IOS_ERROR_NO_MEMORY = -22,
};

/**
 * A thread's entry point, which IOS_CreateThread starts the thread on.
 *
 * @param arg the argument passed to IOS_CreateThread
 * @return the thread's exit value
 */
typedef void *(*ios_thread_entry)(void *arg);

/**
 * Create a thread in the calling process, stopped until IOS_StartThread.
 *
 * @param entry the function the thread runs, Thumb when its address has bit 0 set
 * @param arg the argument entry receives
 * @param stack_top the end of the stack buffer, since the stack grows down
 * @param stack_size the stack's size in bytes
 * @param priority the thread's priority, below 0x80 and no higher than the caller's, a higher number running first
 * @param detached true to free the thread when it ends, false to keep its exit value for a join
 * @return the thread's id, IOS_ERROR_INVALID for a priority out of range, or IOS_ERROR_MAX when every thread is taken
 */
int32_t IOS_CreateThread(ios_thread_entry entry, void *arg, void *stack_top, uint32_t stack_size, int32_t priority,
                         bool detached);

/**
 * Wait for another thread of the calling process to end, then free it.
 *
 * @param thread the thread's id
 * @param exit_value where the thread's exit value is written, or NULL
 * @return IOS_ERROR_OK, or IOS_ERROR_INVALID for the caller itself, a detached thread or one already being joined
 */
int32_t IOS_JoinThread(int32_t thread, uint32_t *exit_value);

/**
 * End a thread, which never returns when it is the caller's own.
 *
 * @param thread the thread's id, or 0 for the calling thread
 * @param exit_value the value a join receives
 * @return a negative result when the thread cannot be ended
 */
int32_t IOS_CancelThread(int32_t thread, uint32_t exit_value);

/**
 * Get the calling thread's id.
 *
 * @return the thread's id
 */
int32_t IOS_GetThreadId(void);

/**
 * Get the calling thread's process ID.
 *
 * @return the process ID
 */
int32_t IOS_GetProcessId(void);

/**
 * Start a thread IOS_CreateThread made.
 *
 * @param thread the thread's id
 * @return IOS_ERROR_OK or a negative result
 */
int32_t IOS_StartThread(int32_t thread);

/**
 * Stop a thread of the calling process from running.
 *
 * @param thread the thread's id, or 0 for the calling thread
 * @return IOS_ERROR_OK, or IOS_ERROR_INVALID for a thread out of range, in another process or not running
 */
int32_t IOS_SuspendThread(int32_t thread);

/**
 * Let other threads of the same priority run before the calling thread continues.
 */
void IOS_YieldThread(void);

/**
 * Get a thread's priority.
 *
 * @param thread the thread's id, or 0 for the calling thread
 * @return the priority, or IOS_ERROR_INVALID for a thread out of range or in another process
 */
int32_t IOS_GetThreadPriority(int32_t thread);

/**
 * Change a thread's priority, which takes effect at once.
 *
 * @param thread the thread's id, or 0 for the calling thread
 * @param priority the new priority, below 0x80 and no higher than the process's highest
 * @return IOS_ERROR_OK, or IOS_ERROR_INVALID for a thread or priority out of range or a thread in another process
 */
int32_t IOS_SetThreadPriority(int32_t thread, int32_t priority);

/**
 * Create a message queue on a buffer the caller owns.
 *
 * @param buffer storage for the messages, one 32-bit word each
 * @param count the number of messages the buffer holds
 * @return the queue's id or a negative result
 */
int32_t IOS_CreateMessageQueue(uint32_t *buffer, uint32_t count);

/**
 * Free a message queue, waking every thread that waits on it with an error.
 *
 * @param queue the queue's id
 * @return IOS_ERROR_OK, IOS_ERROR_ACCESS for another process's queue, or IOS_ERROR_INVALID
 */
int32_t IOS_DestroyMessageQueue(int32_t queue);

/**
 * Add a message to the back of a queue of the calling process.
 *
 * @param queue the queue's id
 * @param message the message
 * @param flags 0 to wait while the queue is full, or 1 to return IOS_ERROR_QUEUE_FULL
 * @return IOS_ERROR_OK, IOS_ERROR_ACCESS for another process's queue, or another negative result
 */
int32_t IOS_SendMessage(int32_t queue, uint32_t message, uint32_t flags);

/**
 * Add a message to the front of a queue of the calling process, so it is received next.
 *
 * @param queue the queue's id
 * @param message the message
 * @param flags 0 to wait while the queue is full, or 1 to return IOS_ERROR_QUEUE_FULL
 * @return IOS_ERROR_OK, IOS_ERROR_ACCESS for another process's queue, or another negative result
 */
int32_t IOS_JamMessage(int32_t queue, uint32_t message, uint32_t flags);

/**
 * Take the next message from a queue.
 *
 * @param queue the queue's id
 * @param message where the message is written
 * @param flags 0 to wait while the queue is empty
 * @return IOS_ERROR_OK or a negative result
 */
int32_t IOS_ReceiveMessage(int32_t queue, uint32_t *message, uint32_t flags);

/**
 * Post a message to a queue each time a hardware event fires.
 *
 * @param event the event's number
 * @param queue the queue's id
 * @param message the message posted
 * @return IOS_ERROR_OK or a negative result
 */
int32_t IOS_HandleEvent(uint32_t event, int32_t queue, uint32_t message);

/**
 * Stop posting messages for a hardware event IOS_HandleEvent set up.
 *
 * @param event the event's number
 * @return IOS_ERROR_OK, IOS_ERROR_ACCESS for another process's event, or IOS_ERROR_INVALID
 */
int32_t IOS_UnregisterEventHandler(uint32_t event);

/**
 * Create a timer that posts a message to a queue.
 *
 * @param time_us the wait before the first message, in microseconds
 * @param repeat_us the wait between later messages, or 0 for one message
 * @param queue the queue's id
 * @param message the message posted
 * @return the timer's id or a negative result
 */
int32_t IOS_CreateTimer(uint32_t time_us, uint32_t repeat_us, int32_t queue, uint32_t message);

/**
 * Start a stopped timer again with new waits, keeping its queue and message.
 *
 * @param timer the timer's id
 * @param time_us the wait before the first message, in microseconds
 * @param repeat_us the wait between later messages, or 0 for one message
 * @return IOS_ERROR_OK, IOS_ERROR_ACCESS for another process's timer, or IOS_ERROR_INVALID for one still running
 */
int32_t IOS_RestartTimer(int32_t timer, uint32_t time_us, uint32_t repeat_us);

/**
 * Stop a timer without freeing it, so IOS_RestartTimer can start it again.
 *
 * @param timer the timer's id
 * @return IOS_ERROR_OK, IOS_ERROR_ACCESS for another process's timer, or IOS_ERROR_INVALID
 */
int32_t IOS_StopTimer(int32_t timer);

/**
 * Stop and free a timer.
 *
 * @param timer the timer's id
 * @return IOS_ERROR_OK or a negative result
 */
int32_t IOS_DestroyTimer(int32_t timer);

/**
 * Read the kernel clock, which counts about 1909 ticks a millisecond.
 *
 * @return the clock's value
 */
uint32_t IOS_GetTime(void);

/**
 * Register a device path whose requests arrive on a queue as struct ipcmessage pointers.
 *
 * @param path the device path, which every open path it is a prefix of matches
 * @param queue the queue's id
 * @return IOS_ERROR_OK or a negative result
 */
int32_t IOS_RegisterResourceManager(const char *path, int32_t queue);

/**
 * Open a device, waiting for its resource manager to answer.
 *
 * @param path the device path
 * @param mode the open mode
 * @return a descriptor, or a negative result such as IOS_ERROR_NOT_FOUND
 */
int32_t IOS_Open(const char *path, uint32_t mode);

/**
 * Send an ioctl without waiting, the answer posted to a queue as the reply's address.
 *
 * @param fd the descriptor
 * @param request the request number
 * @param in the input buffer
 * @param in_len the input's length in bytes
 * @param out the output buffer
 * @param out_len the output's length in bytes
 * @param queue the queue the reply is posted to
 * @param reply where the kernel writes the result, 32-byte aligned
 * @return IOS_ERROR_OK once sent, or a negative result
 */
int32_t IOS_IoctlAsync(int32_t fd, uint32_t request, void *in, uint32_t in_len, void *out, uint32_t out_len,
                       int32_t queue, struct ipcmessage *reply);

/**
 * Send an ioctlv without waiting, the answer posted to a queue as the reply's address.
 *
 * @param fd the descriptor
 * @param request the request number
 * @param in_count the number of input vectors, first in the array
 * @param out_count the number of output vectors, after the inputs
 * @param vectors the vectors
 * @param queue the queue the reply is posted to
 * @param reply where the kernel writes the result, 32-byte aligned
 * @return IOS_ERROR_OK once sent, or a negative result
 */
int32_t IOS_IoctlvAsync(int32_t fd, uint32_t request, uint32_t in_count, uint32_t out_count, struct ioctlv *vectors,
                        int32_t queue, struct ipcmessage *reply);

/**
 * Answer a request a resource manager received, waking its caller.
 *
 * @param request the request
 * @param result the result the caller gets
 * @return IOS_ERROR_OK or a negative result
 */
int32_t IOS_ResourceReply(struct ipcmessage *request, int32_t result);

/**
 * Drain the AHB write buffer of a device, so its DMA reads what the CPU wrote.
 *
 * @param device the device's AHB number
 * @return IOS_ERROR_OK or a negative result
 */
int32_t IOS_AhbMemFlush(uint32_t device);

/**
 * Let a device's DMA writes land in memory before the CPU reads them.
 *
 * @param device the device's AHB number
 * @return IOS_ERROR_OK or a negative result
 */
int32_t IOS_AhbMemRBInvalidate(uint32_t device);

/**
 * Clear and unmask an SD controller's interrupt, for the one process that owns it.
 *
 * @param controller 0 for the SD slot, 1 for the Wi-Fi card, which only process 11 owns
 * @return IOS_ERROR_OK, IOS_ERROR_ACCESS for another process's controller, or another negative result
 */
int32_t IOS_ClearAndEnableSDIntr(uint32_t controller);

/**
 * Ask for access to an iobuf pool.
 *
 * @param pool the pool, 0 for the only one
 * @return IOS_ERROR_OK
 */
int32_t IOS_AccessIobPool(uint32_t pool);

/**
 * Allocate an iobuf with an empty data window at the start of its buffer.
 *
 * @param pool the pool, 0 for the only one
 * @param size the buffer size needed in bytes, at most 0x1000
 * @return the iobuf, or NULL
 */
struct iobuf *IOS_AllocIobuf(uint32_t pool, uint32_t size);

/**
 * Release an iobuf, freeing it once no other module holds it.
 *
 * @param iobuf the iobuf
 * @return IOS_ERROR_OK or IOS_ERROR_INVALID for a pointer that is not an iobuf
 */
int32_t IOS_FreeIobuf(struct iobuf *iobuf);

/**
 * Grow an iobuf's data at its end.
 *
 * @param iobuf the iobuf
 * @param len the bytes to add
 * @return where the added bytes start, or NULL when the buffer has no room
 */
void *IOS_ExtendIobuf(struct iobuf *iobuf, uint16_t len);

/**
 * Grow an iobuf's data at its start, such as to add a header.
 *
 * @param iobuf the iobuf
 * @param len the bytes to add
 * @return the new start of the data, or NULL when the buffer has no room
 */
void *IOS_PushIob(struct iobuf *iobuf, uint16_t len);

/**
 * Shrink an iobuf's data at its start, such as to strip a header.
 *
 * @param iobuf the iobuf
 * @param len the bytes to remove
 * @return where the removed bytes start, or NULL when the data is shorter than len
 */
void *IOS_PullIob(struct iobuf *iobuf, uint16_t len);

/**
 * Check that a pointer is a live iobuf.
 *
 * @param iobuf the iobuf
 * @return IOS_ERROR_OK or IOS_ERROR_INVALID
 */
int32_t IOS_VerifyIobuf(const struct iobuf *iobuf);

/**
 * Drop the data cache lines over a range without writing them back.
 *
 * @param address the start of the range
 * @param size the range's length in bytes
 */
void IOS_InvalidateDCache(void *address, uint32_t size);

/**
 * Write the data cache lines over a range back to memory.
 *
 * @param address the start of the range
 * @param size the range's length in bytes
 */
void IOS_FlushDCache(const void *address, uint32_t size);

/**
 * Translate an address in the calling process to a physical one.
 *
 * @param address the address
 * @return the physical address
 */
void *IOS_VirtualToPhysical(void *address);
