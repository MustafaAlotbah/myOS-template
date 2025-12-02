; =============================================================================
; myOS Multiboot2 Boot Stub
; =============================================================================
; Entry point for the kernel. Sets up Multiboot2 header, stack, and calls
; the C++ kernel entry point.
;
; @author Mustafa Alotbah
; @copyright myOS Project
; =============================================================================

BITS 32

; =============================================================================
; MULTIBOOT 2 HEADER
; =============================================================================
; Multiboot 2 specification:
; https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html

%define MB2_MAGIC           0xE85250D6          ; Multiboot 2 magic number
%define MB2_ARCHITECTURE    0                   ; i386 protected mode
%define MB2_HEADER_LENGTH   (multiboot2_header_end - multiboot2_header_start)
%define MB2_CHECKSUM        -(MB2_MAGIC + MB2_ARCHITECTURE + MB2_HEADER_LENGTH)

; Tag types
%define MB2_TAG_END             0
%define MB2_TAG_INFO_REQUEST    1
%define MB2_TAG_FRAMEBUFFER     5

; Tag flags
%define MB2_TAG_OPTIONAL        1
%define MB2_TAG_REQUIRED        0

section .multiboot2 align=8
multiboot2_header_start:
    ; Required header fields (16 bytes)
    dd MB2_MAGIC                ; Multiboot 2 magic number
    dd MB2_ARCHITECTURE         ; Architecture (0 = i386 protected mode)
    dd MB2_HEADER_LENGTH        ; Header length
    dd MB2_CHECKSUM             ; Checksum: -(magic + arch + length)

    ; -------- Tag: Information Request --------
    ; Request specific tags from bootloader
    align 8
info_request_tag_start:
    dw MB2_TAG_INFO_REQUEST                          ; Tag type: Information Request (1)
    dw MB2_TAG_OPTIONAL                              ; Tag flags: Optional (1)
    dd info_request_tag_end - info_request_tag_start ; Tag length
    dd 4                                             ; Basic memory info (4 bytes)
    dd 6                                             ; Memory map (6 bytes)
    dd 8                                             ; Framebuffer (8 bytes)
    dd 14                                            ; ACPI old RSDP
    dd 15                                            ; ACPI new RSDP
info_request_tag_end:

    ; -------- Tag: Framebuffer Request --------
    ; Request specific video mode from bootloader
    align 8
framebuffer_tag_start:
    dw MB2_TAG_FRAMEBUFFER                          ; Type: Framebuffer
    dw MB2_TAG_OPTIONAL                             ; Flags: Optional (fallback to defaults)
    dd framebuffer_tag_end - framebuffer_tag_start  ; Tag length
    dd 1024                                         ; Width: 1024 pixels
    dd 768                                          ; Height: 768 pixels
    dd 32                                           ; Depth: 32 bits per pixel (ARGB)
framebuffer_tag_end:

    ; -------- Tag: End --------
    ; Marks the end of the header tags
    align 8
end_tag_start:
    dw MB2_TAG_END                             ; Type: End
    dw MB2_TAG_REQUIRED                        ; Flags: Required
    dd end_tag_end - end_tag_start             ; Size: 8 bytes
end_tag_end:

multiboot2_header_end:

; ============================================================================
; END OF MULTIBOOT 2 HEADER
; ============================================================================

; =============================================================================
; BSS SECTION - Uninitialized Data
; =============================================================================
; Kernel stack: 16 KB
; Note: Stack grows DOWNWARD on x86, so:
;   - kernel_stack_bottom = lowest address (stack limit)
;   - kernel_stack_top    = highest address (initial ESP)
;
; Memory layout:
;   kernel_stack_bottom -> [================] <- ESP moves down as stack grows
;                          [  16 KB stack   ]
;   kernel_stack_top    -> [================] <- Initial ESP points here
; =============================================================================

section .bss align=16
kernel_stack_bottom:            ; Start of stack memory (lowest address)
    resb 16*1024                ; Reserve 16 KB for kernel stack
kernel_stack_top:               ; End of stack memory (highest address, initial ESP)

; =============================================================================
; TEXT SECTION - Executable Code
; =============================================================================

section .text

; Externals
extern kernelEntry

; Exports
global kernel_start
global get_stack_bottom
global get_stack_top
global get_stack_size

; -----------------------------------------------------------------------------
; kernel_start - Entry point called by GRUB
; -----------------------------------------------------------------------------
; When bootloader calls this function:
;   EAX = Multiboot 2 magic number (0x36d76289)
;   EBX = Physical address of Multiboot 2 info structure
;   CS  = 32-bit read/execute code segment with offset 0 and limit 0xFFFFFFFF
;   DS,ES,FS,GS,SS = 32-bit read/write data segment with offset 0 and limit 0xFFFFFFFF
;   A20 gate = enabled
;   CR0 = PE bit set, PG bit clear
;   EFLAGS = VM and IF bits clear
; -----------------------------------------------------------------------------
kernel_start:
    ; Disable interrupts
    cli

    ; Set up stack (ESP points to top, grows downward)
    mov esp, kernel_stack_top

    ; Push parameters for kernelEntry(uint32_t magic, uintptr_t addr)
    ; C calling convention: push right-to-left
    push ebx                    ; Second param: multiboot info address
    push eax                    ; First param: multiboot magic

    ; Call C++ kernel entry
    call kernelEntry

    ; Should never return, but just in case...
.halt:
    cli
    hlt
    jmp .halt

; =============================================================================
; STACK INFO FUNCTIONS
; =============================================================================
; These functions return stack memory layout information for use by C++ code.
; Useful for debugging and memory layout visualization.
; =============================================================================

; -----------------------------------------------------------------------------
; get_stack_bottom - Get the lowest address of the kernel stack
; -----------------------------------------------------------------------------
; Returns: EAX = address of stack bottom (lowest usable stack address)
; -----------------------------------------------------------------------------
get_stack_bottom:
    mov eax, kernel_stack_bottom
    ret

; -----------------------------------------------------------------------------
; get_stack_top - Get the highest address of the kernel stack (initial ESP)
; -----------------------------------------------------------------------------
; Returns: EAX = address of stack top (where ESP starts)
; -----------------------------------------------------------------------------
get_stack_top:
    mov eax, kernel_stack_top
    ret

; -----------------------------------------------------------------------------
; get_stack_size - Get the size of the kernel stack in bytes
; -----------------------------------------------------------------------------
; Returns: EAX = stack size in bytes (16 KB = 16384)
; -----------------------------------------------------------------------------
get_stack_size:
    mov eax, kernel_stack_top
    sub eax, kernel_stack_bottom
    ret