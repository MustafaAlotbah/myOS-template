cmake_minimum_required(VERSION 3.20)
project(myOS C CXX ASM_NASM)

# Enable NASM assembler
enable_language(ASM_NASM)

# Options that affect the whole project
option(BUILD_HOST_TOOLS "Build host-side utilities" ON)

# OS build (cross-compiled)
add_subdirectory(myOS)
