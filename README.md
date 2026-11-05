# libios

CMake library for building Wii IOS modules. It provides the Starlet toolchain, the IOS runtime, and interfaces for some of the stock modules, currently SO and WD.

## Installation

Building needs devkitARM from devkitPro. The toolchain file finds devkitPro through `DEVKITPRO` like devkitPro's own CMake files.

Put libios in your project's source tree, for example as a git submodule.

## Usage

```cmake
cmake_minimum_required(VERSION 3.21)

# Enable assembly for the runtime's .S files
project(mymodule LANGUAGES C ASM)

# Add the libios targets and ios_add_bin_output()
add_subdirectory(libios)

# Build the module and link the IOS runtime
add_executable(mymodule src/main.c)
target_link_libraries(mymodule PRIVATE libios::runtime)

# Set the process ID, thread priority, and stack size for the module's thread
target_compile_definitions(
  mymodule
  PRIVATE IOS_PROCESS_ID=10 IOS_PRIORITY=64 IOS_STACK_SIZE=0x400
)

# Set the address the module loads at
target_link_options(mymodule PRIVATE -Wl,--defsym=load_address=0x13eb0000)

# Write mymodule.bin in the stock IOS layout after each link
ios_add_bin_output(mymodule)
```

```sh
cmake -B build -DCMAKE_TOOLCHAIN_FILE=libios/cmake/starlet.cmake
cmake --build build
```

The build writes `mymodule.bin` to the build directory, laid out like the stock modules, along with a link map.

## Configuration

Each module sets five values. The first three are compile definitions and the last two are linker symbols set with `--defsym`.

| Value              | Set with           | Meaning                                                                    |
|--------------------|--------------------|----------------------------------------------------------------------------|
| `IOS_PROCESS_ID`   | Compile definition | The process the module's thread runs in                                    |
| `IOS_PRIORITY`     | Compile definition | The thread's priority, below 128, with higher numbers run first            |
| `IOS_STACK_SIZE`   | Compile definition | The thread's stack size in bytes                                           |
| `load_address`     | `--defsym`         | The virtual address the module runs at                                     |
| `physical_address` | `--defsym`         | The physical address the module loads at, if different from `load_address` |

A replacement for a stock module usually matches its process ID, priority, stack size, and addresses. See [Stock modules](#stock-modules).

## Stock modules

The modules in IOS58 v6176 use these process IDs, priorities, stack sizes, and addresses. Size is the memory a module takes up from its virtual address, covering its code, data, and BSS.

| Module  | Process ID | Priority | Stack size      | Virtual address | Physical address | Size     |
|---------|------------|----------|-----------------|-----------------|------------------|----------|
| DIP     | 3          | 84       | 0x8000 (32 KiB) | 0x20200000      | 0x139b0000       | 212 KiB  |
| OHCI0   | 4          | 72       | 0x4000 (16 KiB) | 0x138a0000      | 0x138a0000       | 57 KiB   |
| OH1     | 5          | 96       | 0x800 (2 KiB)   | 0x138b0000      | 0x138b0000       | 25 KiB   |
| EHCI    | 6          | 97       | 0x2000 (8 KiB)  | 0x138c0000      | 0x138c0000       | 111 KiB  |
| SDI     | 7          | 36       | 0x800 (2 KiB)   | 0x20400000      | 0x13a90000       | 19 KiB   |
| ETH     | 8          | 64       | 0x8000 (32 KiB) | 0x13aa0000      | 0x13aa0000       | 66 KiB   |
| SO      | 9          | 56       | 0x800 (2 KiB)   | 0x13b40000      | 0x13b40000       | 1022 KiB |
| WD      | 10         | 64       | 0x400 (1 KiB)   | 0x13eb0000      | 0x13eb0000       | 111 KiB  |
| WL      | 11         | 72       | 0x3000 (12 KiB) | 0x13ed0000      | 0x13ed0000       | 653 KiB  |
| KD      | 12         | 28       | 0x1000 (4 KiB)  | 0x13db0000      | 0x13db0000       | 978 KiB  |
| NCD     | 13         | 64       | 0x400 (1 KiB)   | 0x13d90000      | 0x13d90000       | 65 KiB   |
| STM     | 14         | 120      | 0x400 (1 KiB)   | 0x20300000      | 0x13d80000       | 30 KiB   |
| SSL     | 16         | 28       | 0x1000 (4 KiB)  | 0x13cc0000      | 0x13cc0000       | 727 KiB  |
| USB     | 17         | 71       | 0xe0c (3.5 KiB) | 0x13650000      | 0x13650000       | 23 KiB   |
| USB_HID | 17         | 70       | 0xc00 (3 KiB)   | 0x13658000      | 0x13658000       | 24 KiB   |
| USB_VEN | 17         | 70       | 0xdc0 (3.4 KiB) | 0x13664000      | 0x13664000       | 21 KiB   |
| USB_HUB | 17         | 70       | 0xc60 (3.1 KiB) | 0x1366c000      | 0x1366c000       | 14 KiB   |

## License

libios is released under the MIT License. See [LICENSE](LICENSE).
