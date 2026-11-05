OUTPUT_FORMAT("elf32-bigarm")
OUTPUT_ARCH(arm)
ENTRY(main)

/* Each module sets load_address, and physical_address when the kernel maps it elsewhere */
physical_address = DEFINED(physical_address) ? physical_address : load_address;
physical_offset = physical_address - load_address;

/* Program headers in the stock modules' order, each in MMU domain 15 */
PHDRS {
	phdrs   PT_PHDR PHDRS FLAGS(0x00f00000);
	note    PT_NOTE FLAGS(0x00f00000);
	headers PT_LOAD PHDRS FLAGS(0x00f00000);
	text    PT_LOAD FLAGS(0x00f00005);
	data    PT_LOAD FLAGS(0x00f00006);
	bss     PT_LOAD FLAGS(0x00f00006);
}

SECTIONS {
	/* Loader table, after the program headers at address 0 */
	.ios_info_table SIZEOF_HEADERS - 0x34 : {
		KEEP(*(.ios_info_table))
	} :headers :note

	/* Code and constants from the load address */
	. = load_address;
	.text : AT(ADDR(.text) + physical_offset) {
		*(.text*)
		*(.glue_7)
		*(.glue_7t)
		. = ALIGN(4);
	} :text

	.rodata : AT(ADDR(.rodata) + physical_offset) {
		*(.rodata*)
		. = ALIGN(4);
	} :text

	/* Data and BSS each on the next page, like the stock modules */
	. = ALIGN(0x1000);
	.data : AT(ADDR(.data) + physical_offset) {
		*(.data*)
		. = ALIGN(4);
	} :data

	. = ALIGN(0x1000);
	.bss : AT(ADDR(.bss) + physical_offset) {
		*(.bss*)
		*(COMMON)
		KEEP(*(.ios_bss))
		. = ALIGN(4);
	} :bss
}
