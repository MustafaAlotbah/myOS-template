# cmake/nasm.cmake
# NASM assembler configuration for 32-bit ELF output
#
# Include this after project() and enable_language(ASM_NASM)

# Ensure NASM outputs 32-bit ELF format
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)

# Custom compile command for NASM
# -f elf32: Output 32-bit ELF
# -g: Include debug symbols
set(CMAKE_ASM_NASM_COMPILE_OBJECT 
    "<CMAKE_ASM_NASM_COMPILER> -f elf32 -g -o <OBJECT> <SOURCE>")

# NASM include directories (if needed)
# set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -I${CMAKE_SOURCE_DIR}/include/")

message(STATUS "NASM configured for elf32 output")

