# Set minimum CMake version
cmake_minimum_required(VERSION 3.21)

# Configure the project, which compiles nothing itself since every target is an interface
project(libios LANGUAGES C)

# Give a target the IOS headers, the request struct, the syscall prototypes and the stock module interfaces
add_library(ios_headers INTERFACE)
target_include_directories(ios_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(libios::headers ALIAS ios_headers)

# Make a target an IOS module, with its thread note, the kernel syscalls, the missing libc and the loader layout
add_library(ios_runtime INTERFACE)
target_sources(
  ios_runtime
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/thread.S
    ${CMAKE_CURRENT_SOURCE_DIR}/src/syscalls.S
    ${CMAKE_CURRENT_SOURCE_DIR}/src/string.c
)
target_link_libraries(ios_runtime INTERFACE libios::headers)
target_link_options(ios_runtime INTERFACE -Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/link.ld)

add_library(libios::runtime ALIAS ios_runtime)

# Write a link map next to each module's ELF
target_link_options(ios_runtime INTERFACE -Wl,-Map,${CMAKE_BINARY_DIR}/$<TARGET_PROPERTY:NAME>.map)

# Relink a module when the linker script changes
set_target_properties(
  ios_runtime
  PROPERTIES INTERFACE_LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/link.ld
)

# Write a module's .bin in the stock IOS layout next to its ELF, after each link
function(ios_add_bin_output target)
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND
      sh ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake/elf2ios.sh ${CMAKE_OBJCOPY}
      $<TARGET_FILE:${target}> ${CMAKE_BINARY_DIR}/${target}.bin
    VERBATIM
  )
endfunction()
