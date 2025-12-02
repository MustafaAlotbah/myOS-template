# kernel CMakeLists.txt
# Kernel-only code (privileged operations)
# Built as OBJECT library since it links directly into kernel.bin

add_library(kernel_core OBJECT
    core/main.cpp
    core/Kernel.cpp
    core/panic.cpp
    core/Timer.cpp
    core/memory/PhysicalMemory.cpp
    core/peripherals/Logger.cpp
    core/peripherals/Display.cpp
)

target_include_directories(kernel_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# SDK includes
target_include_directories(kernel_core PUBLIC
    ${CMAKE_SOURCE_DIR}/sdk/include
)

# Arch includes
target_include_directories(kernel_core PUBLIC
    ${CMAKE_SOURCE_DIR}/arch/x86/include
)

# Libc includes (for vsnprintf in panic)
target_include_directories(kernel_core PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/include
)
