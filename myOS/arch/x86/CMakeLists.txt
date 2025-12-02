# arch/x86 CMakeLists.txt
# x86 architecture-specific code

add_library(arch_x86 OBJECT
    boot/multiboot.asm
)

target_include_directories(arch_x86 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
