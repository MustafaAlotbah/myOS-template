/**
 * @file multiboot2.h
 * @brief Multiboot2 boot protocol structures and helpers
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stdint.h>

namespace myOS::arch::multiboot2 {

    constexpr uint32_t MAGIC = 0x36d76289;

    enum class TagType : uint32_t {
        End              = 0,
        CommandLine      = 1,
        BootloaderName   = 2,
        Module           = 3,
        BasicMemInfo     = 4,
        BIOSBootDevice   = 5,
        MemoryMap        = 6,
        VBEInfo          = 7,
        Framebuffer      = 8,
        ElfSections      = 9,
        APMTable         = 10,
        EFI32            = 11,
        EFI64            = 12,
        SMBIOS           = 13,
        ACPIOld          = 14,
        ACPINew          = 15,
        Network          = 16,
        EFIMemoryMap     = 17,
        EFIBootServices  = 18,
        EFI32ImageHandle = 19,
        EFI64ImageHandle = 20,
        LoadBaseAddr     = 21
    };

    enum class FramebufferType : uint8_t { Indexed = 0, RGB = 1, EGAText = 2 };

    struct Tag {
        uint32_t type;
        uint32_t size;
    };

    struct TagFramebuffer {
        uint32_t type;
        uint32_t size;
        uint64_t framebuffer_addr;
        uint32_t framebuffer_pitch;
        uint32_t framebuffer_width;
        uint32_t framebuffer_height;
        uint8_t framebuffer_bpp;
        uint8_t framebuffer_type;
        uint16_t reserved;
    };

    struct TagBasicMemInfo {
        uint32_t type;
        uint32_t size;
        uint32_t mem_lower;  // KB of lower memory (below 1MB)
        uint32_t mem_upper;  // KB of upper memory (above 1MB)
    };

    enum class MemoryType : uint32_t { Available = 1, Reserved = 2, ACPIReclaimable = 3, ACPINVS = 4, BadRAM = 5 };

    struct MemoryMapEntry {
        uint64_t base_addr;
        uint64_t length;
        uint32_t type;
        uint32_t reserved;
    };

    struct TagMemoryMap {
        uint32_t type;
        uint32_t size;
        uint32_t entry_size;
        uint32_t entry_version;
        // Followed by MemoryMapEntry entries
    };

    /**
     * @brief Validate Multiboot2 magic number
     */
    inline bool isValid(uint32_t magic) { return magic == MAGIC; }

    /**
     * @brief Find a tag by type in Multiboot2 info
     */
    inline const Tag* findTag(uint32_t multibootAddr, TagType tagType) {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(multibootAddr);
        ptr += 8;  // Skip total_size and reserved

        while (true) {
            const Tag* tag = reinterpret_cast<const Tag*>(ptr);

            if (tag->type == static_cast<uint32_t>(TagType::End)) { break; }

            if (tag->type == static_cast<uint32_t>(tagType)) { return tag; }

            // Move to next tag (8-byte aligned)
            ptr += (tag->size + 7) & ~7;
        }

        return nullptr;
    }

    /**
     * @brief Find framebuffer tag in Multiboot2 info
     */
    inline const TagFramebuffer* findFramebuffer(uint32_t multibootAddr) { return reinterpret_cast<const TagFramebuffer*>(findTag(multibootAddr, TagType::Framebuffer)); }

    /**
     * @brief Find basic memory info tag
     */
    inline const TagBasicMemInfo* findBasicMemInfo(uint32_t multibootAddr) { return reinterpret_cast<const TagBasicMemInfo*>(findTag(multibootAddr, TagType::BasicMemInfo)); }

    /**
     * @brief Find memory map tag
     */
    inline const TagMemoryMap* findMemoryMap(uint32_t multibootAddr) { return reinterpret_cast<const TagMemoryMap*>(findTag(multibootAddr, TagType::MemoryMap)); }

    /**
     * @brief Get total available memory from memory map
     * @return Total available memory in bytes
     */
    inline uint64_t getTotalAvailableMemory(uint32_t multibootAddr) {
        const auto* mmap = findMemoryMap(multibootAddr);
        if (!mmap) {
            // Fall back to basic mem info
            const auto* basic = findBasicMemInfo(multibootAddr);
            if (basic) { return (static_cast<uint64_t>(basic->mem_upper) + 1024) * 1024; }
            return 0;
        }

        uint64_t total           = 0;
        const uint8_t* entry_ptr = reinterpret_cast<const uint8_t*>(mmap) + sizeof(TagMemoryMap);
        const uint8_t* end_ptr   = reinterpret_cast<const uint8_t*>(mmap) + mmap->size;

        while (entry_ptr < end_ptr) {
            const auto* entry = reinterpret_cast<const MemoryMapEntry*>(entry_ptr);
            if (entry->type == static_cast<uint32_t>(MemoryType::Available)) { total += entry->length; }
            entry_ptr += mmap->entry_size;
        }

        return total;
    }

    /**
     * @brief Get highest usable memory address
     */
    inline uint64_t getHighestMemoryAddress(uint32_t multibootAddr) {
        const auto* mmap = findMemoryMap(multibootAddr);
        if (!mmap) {
            const auto* basic = findBasicMemInfo(multibootAddr);
            if (basic) { return (static_cast<uint64_t>(basic->mem_upper) + 1024) * 1024; }
            return 0;
        }

        uint64_t highest         = 0;
        const uint8_t* entry_ptr = reinterpret_cast<const uint8_t*>(mmap) + sizeof(TagMemoryMap);
        const uint8_t* end_ptr   = reinterpret_cast<const uint8_t*>(mmap) + mmap->size;

        while (entry_ptr < end_ptr) {
            const auto* entry = reinterpret_cast<const MemoryMapEntry*>(entry_ptr);
            if (entry->type == static_cast<uint32_t>(MemoryType::Available)) {
                uint64_t end = entry->base_addr + entry->length;
                if (end > highest) highest = end;
            }
            entry_ptr += mmap->entry_size;
        }

        return highest;
    }

}  // namespace myOS::arch::multiboot2
