# cmake/output-dirs.cmake
# Output directory configuration
#
# All build outputs go to the project root's bin/ folder

# Get the project root (parent of myOS)
get_filename_component(PROJECT_ROOT "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)

# Output directories at project root
set(OUTPUT_DIR "${PROJECT_ROOT}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/lib)

# Also set for each build type (Debug, Release, etc.)
foreach(TYPE IN ITEMS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${TYPE} ${OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${TYPE} ${OUTPUT_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${TYPE} ${OUTPUT_DIR}/lib)
endforeach()

# Export for use in other cmake files
set(PROJECT_ROOT ${PROJECT_ROOT} CACHE INTERNAL "Project root directory")
set(OUTPUT_DIR ${OUTPUT_DIR} CACHE INTERNAL "Output directory")

message(STATUS "Project root: ${PROJECT_ROOT}")
message(STATUS "Output directory: ${OUTPUT_DIR}")
