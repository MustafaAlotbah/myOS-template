# cmake/toolchains/native-gcc-12-m32.cmake
# Toolchain file for building 32-bit kernel with native GCC-12
#
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/native-gcc-12-m32.cmake ../myOS

# Target system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)

# Compilers - must use absolute paths or names that exist in PATH
set(CMAKE_C_COMPILER gcc-12)
set(CMAKE_CXX_COMPILER g++-12)

# Initial flags for 32-bit compilation
set(CMAKE_C_FLAGS_INIT "-m32")
set(CMAKE_CXX_FLAGS_INIT "-m32")

# Disable compiler checks for freestanding environment
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

