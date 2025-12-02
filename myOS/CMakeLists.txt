# myOS CMakeLists.txt
# Root build configuration for myOS kernel
#
# Build (from project root):
#   mkdir build && cd build
#   cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/native-gcc-12-m32.cmake ../myOS
#   cmake --build . --target myOS
#
# Or for cross-compiler:
#   cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/i686-elf-cross.cmake ../myOS

cmake_minimum_required(VERSION 3.16)

# NOTE: Toolchain file must be specified via -DCMAKE_TOOLCHAIN_FILE=...
#       Do NOT include() toolchain files here - it causes infinite reconfigure loops!

# Verify a toolchain was provided
if(NOT CMAKE_TOOLCHAIN_FILE)
    message(WARNING "No toolchain file specified!")
    message(WARNING "Use: cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/native-gcc-12-m32.cmake ../myOS")
    message(WARNING "Attempting to continue with default compiler...")
endif()

# Project definition
project(myOS VERSION 0.1.0 LANGUAGES C CXX ASM_NASM)

# Include CMake modules (these do NOT modify CMAKE_C_COMPILER or CMAKE_CXX_COMPILER)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/kernel-flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/nasm.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/output-dirs.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/iso.cmake)

# Enable compile_commands.json for IDE indexing
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# SUBDIRECTORIES
# ============================================================================

# SDK - builds libmyos-sdk.a
add_subdirectory(sdk)

# Kernel core - builds object files
add_subdirectory(kernel)

# Architecture - builds object files  
add_subdirectory(arch/x86)

# Libs (libc) - builds libmyos-libc.a
add_subdirectory(libs)

# ============================================================================
# KERNEL BINARY
# ============================================================================

add_executable(kernel.bin
    $<TARGET_OBJECTS:kernel_core>
    $<TARGET_OBJECTS:arch_x86>
)

# Link against static libraries
target_link_libraries(kernel.bin PRIVATE
    myos-sdk
    myos-libc
)

# Linker configuration
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker/x86.ld)
set(CMAKE_CXX_LINK_EXECUTABLE "ld -m elf_i386 -T ${LINKER_SCRIPT} -nostdlib <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# Include directories
target_include_directories(kernel.bin PRIVATE
    ${CMAKE_SOURCE_DIR}/sdk/include
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/arch/x86/include
)

# ============================================================================
# BOOTABLE ISO
# ============================================================================

create_bootable_iso(myOS 
    ${OUTPUT_DIR}/kernel.bin 
    ${CMAKE_SOURCE_DIR}/platform/grub/grub.cfg
)

# ============================================================================
# BUILD INFO
# ============================================================================

message(STATUS "")
message(STATUS "=== myOS Build Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Toolchain: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Libraries:")
message(STATUS "  - ${OUTPUT_DIR}/lib/libmyos-sdk.a")
message(STATUS "  - ${OUTPUT_DIR}/lib/libmyos-libc.a")
message(STATUS "Binaries:")
message(STATUS "  - ${OUTPUT_DIR}/kernel.bin")
message(STATUS "  - ${OUTPUT_DIR}/myOS.iso")
message(STATUS "================================")
message(STATUS "")
