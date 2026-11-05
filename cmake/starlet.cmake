# Find devkitPro, at its default path unless DEVKITPRO names another
if(DEFINED ENV{DEVKITPRO})
  set(DEVKITPRO $ENV{DEVKITPRO})
else()
  set(DEVKITPRO /opt/devkitpro)
endif()

# Build with devkitARM
include(${DEVKITPRO}/cmake/devkitARM.cmake)

# Take no build type flags from devkitPro, since the flags below set the optimization and no debug info
set(CMAKE_BUILD_TYPE "" CACHE STRING "")

# Compile freestanding Thumb for the Starlet, sized for its small memory
set(STARLET_ARCH_FLAGS "-mcpu=arm926ej-s -mbig-endian -mthumb -mthumb-interwork")
set(
  STARLET_COMPILE_FLAGS
  "${STARLET_ARCH_FLAGS} -Os -ffreestanding -fno-builtin -ffunction-sections -fdata-sections -fomit-frame-pointer"
)
set(CMAKE_C_FLAGS_INIT ${STARLET_COMPILE_FLAGS})
set(CMAKE_ASM_FLAGS_INIT ${STARLET_COMPILE_FLAGS})

# Link without the C runtime, dropping unused sections
set(
  CMAKE_EXE_LINKER_FLAGS_INIT
  "${STARLET_ARCH_FLAGS} -nostartfiles -nostdlib -Wl,--gc-sections -Wl,-z,max-page-size=4"
)

# Link libgcc last, in place of the C runtime, for the helpers GCC calls in Thumb code
set(CMAKE_C_STANDARD_LIBRARIES "-lgcc")
