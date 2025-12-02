# cmake/iso.cmake
# ISO image creation utilities
#
# Provides function to create bootable ISO images

# Function to create a bootable ISO from a kernel binary
# Usage: create_bootable_iso(target_name kernel_binary grub_config)
function(create_bootable_iso TARGET_NAME KERNEL_BIN GRUB_CONFIG)
    set(ISO_DIR ${OUTPUT_DIR}/iso)
    set(ISO_FILE ${OUTPUT_DIR}/${TARGET_NAME}.iso)
    
    add_custom_target(${TARGET_NAME} ALL
        DEPENDS kernel.bin
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ISO_DIR}/boot/grub
        COMMAND ${CMAKE_COMMAND} -E copy ${KERNEL_BIN} ${ISO_DIR}/boot/
        COMMAND ${CMAKE_COMMAND} -E copy ${GRUB_CONFIG} ${ISO_DIR}/boot/grub/
        COMMAND grub-mkrescue -o ${ISO_FILE} ${ISO_DIR}
        COMMENT "Creating bootable ISO: ${ISO_FILE}"
    )
    
    message(STATUS "ISO target '${TARGET_NAME}' will be created at: ${ISO_FILE}")
endfunction()
