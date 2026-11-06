// ARM syscall stubs
	.section ".text"
	.arm

// Syscall stub macro
.macro syscall num, name
	.global \name
	.type \name, %function
\name:
	.long	0xe6000010 + (\num << 5)
	bx	lr
.endm

// Kernel syscalls, with prototypes in ios/syscalls.h
	syscall 0x00, IOS_CreateThread
	syscall 0x01, IOS_JoinThread
	syscall 0x02, IOS_CancelThread
	syscall 0x03, IOS_GetThreadId
	syscall 0x04, IOS_GetProcessId
	syscall 0x05, IOS_StartThread
	syscall 0x06, IOS_SuspendThread
	syscall 0x07, IOS_YieldThread
	syscall 0x08, IOS_GetThreadPriority
	syscall 0x09, IOS_SetThreadPriority
	syscall 0x0a, IOS_CreateMessageQueue
	syscall 0x0b, IOS_DestroyMessageQueue
	syscall 0x0c, IOS_SendMessage
	syscall 0x0d, IOS_JamMessage
	syscall 0x0e, IOS_ReceiveMessage
	syscall 0x0f, IOS_HandleEvent
	syscall 0x10, IOS_UnregisterEventHandler
	syscall 0x11, IOS_CreateTimer
	syscall 0x12, IOS_RestartTimer
	syscall 0x13, IOS_StopTimer
	syscall 0x14, IOS_DestroyTimer
	syscall 0x15, IOS_GetTime
	syscall 0x16, IOS_CreateHeap
	syscall 0x17, IOS_DestroyHeap
	syscall 0x18, IOS_Alloc
	syscall 0x19, IOS_AllocAligned
	syscall 0x1a, IOS_Free
	syscall 0x1b, IOS_RegisterResourceManager
	syscall 0x1c, IOS_Open
	syscall 0x1d, IOS_Close
	syscall 0x1e, IOS_Read
	syscall 0x1f, IOS_Write
	syscall 0x20, IOS_Seek
	syscall 0x21, IOS_Ioctl
	syscall 0x22, IOS_Ioctlv
	syscall 0x23, IOS_OpenAsync
	syscall 0x24, IOS_CloseAsync
	syscall 0x25, IOS_ReadAsync
	syscall 0x26, IOS_WriteAsync
	syscall 0x27, IOS_SeekAsync
	syscall 0x28, IOS_IoctlAsync
	syscall 0x29, IOS_IoctlvAsync
	syscall 0x2a, IOS_ResourceReply
	syscall 0x2f, IOS_AhbMemFlush
	syscall 0x30, IOS_AhbMemRBInvalidate
	syscall 0x31, IOS_ClearAndEnableIPCIOPIntr
	syscall 0x32, IOS_ClearAndEnableDIIntr
	syscall 0x33, IOS_ClearAndEnableSDIntr
	syscall 0x34, IOS_ClearAndEnableEvent
	syscall 0x35, IOS_AccessIobPool
	syscall 0x36, IOS_AllocIobuf
	syscall 0x37, IOS_FreeIobuf
	syscall 0x3a, IOS_ExtendIobuf
	syscall 0x3b, IOS_PushIob
	syscall 0x3c, IOS_PullIob
	syscall 0x3d, IOS_VerifyIobuf
	syscall 0x3f, IOS_InvalidateDCache
	syscall 0x40, IOS_FlushDCache
	syscall 0x4f, IOS_VirtualToPhysical
