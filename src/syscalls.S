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
	syscall 0x02, IOS_CancelThread
	syscall 0x05, IOS_StartThread
	syscall 0x0a, IOS_CreateMessageQueue
	syscall 0x0e, IOS_ReceiveMessage
	syscall 0x0f, IOS_HandleEvent
	syscall 0x11, IOS_CreateTimer
	syscall 0x14, IOS_DestroyTimer
	syscall 0x15, IOS_GetTime
	syscall 0x1b, IOS_RegisterResourceManager
	syscall 0x1c, IOS_Open
	syscall 0x28, IOS_IoctlAsync
	syscall 0x29, IOS_IoctlvAsync
	syscall 0x2a, IOS_ResourceReply
	syscall 0x2f, IOS_AhbMemFlush
	syscall 0x30, IOS_AhbMemRBInvalidate
	syscall 0x33, IOS_ClearAndEnableSDIntr
	syscall 0x36, IOS_AllocIobuf
	syscall 0x3d, IOS_VerifyIobuf
	syscall 0x3f, IOS_InvalidateDCache
	syscall 0x40, IOS_FlushDCache
	syscall 0x4f, IOS_VirtualToPhysical
