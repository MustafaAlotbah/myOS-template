# SDK CMakeLists.txt
# Builds the myOS SDK as a static library
# This library can be linked by both kernel and userspace applications

add_library(myos-sdk STATIC
    src/Color.cpp
    src/Font.cpp
    src/FrameBuffer.cpp
    src/Brush.cpp
    src/TextRenderer.cpp
)

target_include_directories(myos-sdk PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set output name (produces libmyos-sdk.a)
set_target_properties(myos-sdk PROPERTIES
    OUTPUT_NAME "myos-sdk"
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib"
)
