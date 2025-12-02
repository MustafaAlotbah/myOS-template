/**
 * myOS Linker Script for i386 (x86 32-bit)
 * 
 * This script defines the memory layout of the kernel binary.
 * It tells the linker how to arrange sections in the output ELF file.
 * 
 * Memory Layout:
 *   0x00000000 - 0x000FFFFF : Low memory (1MB) - BIOS, IVT, VGA, reserved
 *   0x00100000 - 0x00FFFFFF : Extended memory (15MB) - Available
 *   0x01000000 - ...        : Kernel load address (16MB+)
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

/* Entry Point - first code executed (see multiboot.asm) */
ENTRY(kernel_start)

/* Output format: 32-bit ELF (Executable and Linkable Format) */
OUTPUT_FORMAT(elf32-i386)

/* Architecture: Intel 80386 (i386) */
OUTPUT_ARCH(i386:i386)

/* SECTION LAYOUT
 * 
 * The kernel is organized into these sections:
 *   .text   - Executable code and read-only data
 *   .data   - Initialized global/static variables
 *   .bss    - Uninitialized global/static variables (zeroed at runtime)
 * 
 * Each section is page-aligned (4KB) for paging compatibility.
 */
SECTIONS {	
    /* KERNEL BASE ADDRESS
     *
     * Place kernel at 16MB (0x01000000) to avoid low memory reserved areas:
     *   - Real-mode interrupt vector table (IVT) at 0x0000-0x03FF
     *   - BIOS data area (BDA) at 0x0400-0x04FF
     *   - Video memory for VGA at 0xA0000-0xBFFFF
     *   - ROM area at 0xC0000-0xFFFFF
     * 
     * This also leaves room for:
     *   - Back buffer for double-buffering graphics
     *   - DMA buffers for hardware drivers
     */
    . = 0x01000000;
    __kernel_start = .;

    /* TEXT SECTION - Executable Code
     *
     * Contains:
     *   - Multiboot2 header (MUST be first, within first 32KB)
     *   - Read-only data (string literals, const tables)
     *   - Executable code (.text)
     * 
     * Page-aligned (4KB) for memory protection via paging.
     */
    .text ALIGN(4096) : {
        __text_start = .;

        /* Multiboot2 header - MUST be at the beginning of the binary
         * The bootloader (GRUB) searches for this header in the first 32KB.
         * See multiboot.asm for header definition. */
        __multiboot_start = .;
        KEEP(*(.multiboot2))
        __multiboot_end = .;

        /* Read-only data: string literals, const arrays, vtables, etc.
         * Placed here (in .text) so it's in a read-only page when paging is enabled. */
        __rodata_start = .;
        *(.rodata*)
        __rodata_end = .;

        /* Executable code from all source files */
        __code_start = .;
        *(.text*)
        __code_end = .;

        __text_end = .;
    }

    /* DATA SECTION - Initialized Data
     *
     * Contains:
     *   - C++ global constructors (init_array)
     *   - Initialized global and static variables
     * 
     * Page-aligned for memory protection.
     */
    .data ALIGN(4096) : {
        __data_start = .;

        /* C++ Global Constructors
         * These are function pointers to constructors for global objects.
         * The kernel calls these after basic initialization.
         * 
         * first_constructor: Start of constructor array
         * last_constructor:  End of constructor array
         * 
         * Usage in kernel:
         *   for (void (**ctor)() = &first_constructor; ctor != &last_constructor; ++ctor)
         *       (*ctor)();
         */
        __ctors_start = .;
        first_constructor = .;
        KEEP(*(.init_array))
        /* Sort by priority - lower numbers run first */
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        last_constructor = .;
        __ctors_end = .;

        /* Note: Destructors (fini_array) are discarded - kernel never exits */

        /* Initialized global and static variables */
        *(.data*)

        __data_end = .;
    }

    /* BSS SECTION - Uninitialized Data
     *
     * Contains:
     *   - Uninitialized global and static variables
     *   - COMMON symbols (C-style uninitialized globals)
     * 
     * This section takes no space in the binary file but is allocated
     * at runtime. The bootloader/kernel zeroes this memory.
     * 
     * Page-aligned for memory protection.
     */
    .bss ALIGN(4096) : {
        __bss_start = .;
        
        *(.bss*)
        *(COMMON)
        
        __bss_end = .;
    }

    /* KERNEL END MARKER
     *
     * Marks the end of the kernel in memory.
     * Used by the physical memory manager to know where free memory begins.
     * 
     * Multiple aliases for compatibility:
     *   __kernel_end - Primary symbol (recommended)
     *   _end, end    - Traditional Unix symbols
     */
    . = ALIGN(4096);
    __kernel_end = .;
    _end = .;
    end = .;

    /* DISCARDED SECTIONS
     *
     * These sections are not needed in the final binary:
     *   - .fini_array  : Destructors (kernel never exits)
     *   - .comment     : Compiler/linker version strings
     *   - .note*       : Build ID and other notes
     *   - .eh_frame*   : Exception handling (disabled in kernel)
     */
    /DISCARD/ : {
        *(.fini_array*)
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}

/* ============================================================================
 * SYMBOL SUMMARY
 * ============================================================================
 * 
 * Kernel boundaries:
 *   __kernel_start    - Start of kernel (0x01000000)
 *   __kernel_end      - End of kernel (first free address)
 * 
 * Section boundaries:
 *   __text_start/end  - Code section
 *   __data_start/end  - Initialized data section
 *   __bss_start/end   - Uninitialized data section
 * 
 * Subsection boundaries:
 *   __multiboot_start/end - Multiboot2 header
 *   __rodata_start/end    - Read-only data
 *   __code_start/end      - Executable code
 *   __ctors_start/end     - C++ constructors
 * 
 * Constructor pointers:
 *   first_constructor  - First global constructor
 *   last_constructor   - Past-the-end of constructors
 */
