#!/bin/sh
# Write an IOS module ELF in the stock layout: elf2ios.sh OBJCOPY IN.elf OUT.bin
set -e

# Drop the symbols and the section table, which the kernel never reads
"$1" --strip-all --strip-section-headers "$2" "$3"

# Set OS/ABI 0x61, which the kernel's module loader requires, and ABI version 1 like the stock modules
printf '\141\001' | dd of="$3" bs=1 seek=7 conv=notrunc 2>/dev/null

# Set the flags to 0x606 like the stock modules
printf '\000\000\006\006' | dd of="$3" bs=1 seek=36 conv=notrunc 2>/dev/null
