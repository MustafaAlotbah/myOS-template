# cmake/kernel-flags.cmake
# Compiler flags for freestanding kernel code
#
# Include this after project() to set up kernel compilation flags
# NOTE: Do NOT set CMAKE_C_COMPILER or CMAKE_CXX_COMPILER here!
#       Use a toolchain file instead.

# Print which compiler is being used (diagnostic)
message(STATUS "Using C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Using C++ compiler: ${CMAKE_CXX_COMPILER}")

# Freestanding environment flags
set(KERNEL_COMMON_FLAGS "-ffreestanding -nostdlib -g")

# C flags
set(KERNEL_C_FLAGS "${KERNEL_COMMON_FLAGS}")

# C++ flags - no exceptions, no RTTI (not available in freestanding)
set(KERNEL_CXX_FLAGS "${KERNEL_COMMON_FLAGS} -fno-exceptions -fno-rtti")

# Warning flags (optional but recommended)
# Note: -Wpedantic removed because anonymous structs are useful in OS code
set(KERNEL_WARNING_FLAGS "-Wall -Wextra")

# Apply flags (append to any existing flags from toolchain)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${KERNEL_C_FLAGS} ${KERNEL_WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KERNEL_CXX_FLAGS} ${KERNEL_WARNING_FLAGS}")

# Linker flags
set(KERNEL_LINK_FLAGS "-nostdlib")

message(STATUS "Kernel C flags: ${CMAKE_C_FLAGS}")
message(STATUS "Kernel C++ flags: ${CMAKE_CXX_FLAGS}")
