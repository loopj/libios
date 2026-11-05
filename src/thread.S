// IOS note tags
#define TAG_PROCESS_ID  0x0b
#define TAG_ENTRY       0x09
#define TAG_PRIORITY    0x7d
#define TAG_STACK_SIZE  0x7e
#define TAG_STACK_TOP   0x7f

// Process ID, thread priority and stack size in bytes, which each module sets
#ifndef IOS_PROCESS_ID
#error "IOS_PROCESS_ID is not set for the module"
#endif
#ifndef IOS_PRIORITY
#error "IOS_PRIORITY is not set for the module"
#endif
#ifndef IOS_STACK_SIZE
#error "IOS_STACK_SIZE is not set for the module"
#endif

// Thread stack and the IOS note that starts the module's thread
	.section ".ios_bss", "a", %nobits
	.align 4
	.space IOS_STACK_SIZE
	.balign 8
ios_thread_stack:
	.section ".ios_info_table", "a", %progbits
	.long	0x0
	.long	0x28
	.long	0x6
	.long	TAG_PROCESS_ID
	.long	IOS_PROCESS_ID
	.long	TAG_ENTRY
	.long	main
	.long	TAG_PRIORITY
	.long	IOS_PRIORITY
	.long	TAG_STACK_SIZE
	.long	IOS_STACK_SIZE
	.long	TAG_STACK_TOP
	.long	ios_thread_stack
