# libs CMakeLists.txt
# C runtime libraries (libc) for freestanding environment
# This library provides standard C functions for both kernel and userspace

add_library(myos-libc STATIC
    # Memory functions
    src/memory.cpp
    
    # String functions
    src/string.cpp
    
    # Standard library (atoi, etc.)
    src/stdlib.cpp
    
    # Character type functions
    src/ctype.cpp
    
    # Standard I/O (sprintf, snprintf, sscanf, etc.)
    src/stdio.cpp
    
    # Math functions (abs, sqrt, sin/cos lookup tables)
    src/math.cpp
    
    # GCC runtime support (64-bit division on 32-bit systems)
    src/libgcc.cpp
)

target_include_directories(myos-libc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set output name (produces libmyos-libc.a)
set_target_properties(myos-libc PROPERTIES
    OUTPUT_NAME "myos-libc"
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib"
)

