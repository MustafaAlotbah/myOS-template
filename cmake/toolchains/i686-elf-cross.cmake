# cmake/toolchains/i686-elf-cross.cmake
# Toolchain file for building kernel with i686-elf cross-compiler
#
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/i686-elf-cross.cmake ../myOS

# Target system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)

# Cross-compilers
set(CMAKE_C_COMPILER i686-elf-gcc)
set(CMAKE_CXX_COMPILER i686-elf-g++)

# Disable compiler checks for freestanding environment
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

