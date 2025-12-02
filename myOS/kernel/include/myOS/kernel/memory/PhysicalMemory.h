/**
 * @file PhysicalMemory.h
 * @brief Physical memory manager using bitmap allocation
 * 
 * KERNEL ONLY - Manages physical RAM as 4KB pages/frames.
 * 
 * Uses a bitmap to track which pages are free or allocated.
 * Each bit represents one 4KB page of physical memory.
 * 
 * Usage:
 *   PhysicalMemory::initialize(memorySize);
 *   void* page = PhysicalMemory::allocateFrame();
 *   PhysicalMemory::freeFrame(page);
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

namespace myOS::kernel {

/**
 * @brief Memory constants
 */
constexpr size_t PAGE_SIZE = 4096;           // 4KB pages
constexpr size_t PAGE_SHIFT = 12;            // log2(4096) = 12

/**
 * @brief Physical memory frame allocator
 * 
 * Manages physical memory using a bitmap. Each bit represents
 * a 4KB frame: 0 = free, 1 = allocated/reserved.
 */
class PhysicalMemory {
public:
    /**
     * @brief Initialize the physical memory manager
     * 
     * @param kernelEnd Address where kernel ends (first usable address)
     * @param memorySize Total physical memory size in bytes
     */
    static void initialize(uintptr_t kernelEnd, size_t memorySize);
    
    /**
     * @brief Allocate a single 4KB frame
     * @return Physical address of allocated frame, or nullptr if OOM
     */
    static void* allocateFrame();
    
    /**
     * @brief Allocate multiple contiguous frames
     * @param count Number of frames to allocate
     * @return Physical address of first frame, or nullptr if OOM
     */
    static void* allocateFrames(size_t count);
    
    /**
     * @brief Free a previously allocated frame
     * @param frame Physical address of frame to free
     */
    static void freeFrame(void* frame);
    
    /**
     * @brief Free multiple contiguous frames
     * @param frame Physical address of first frame
     * @param count Number of frames to free
     */
    static void freeFrames(void* frame, size_t count);
    
    /**
     * @brief Reserve a frame (mark as used without allocating)
     * @param frame Physical address to reserve
     * 
     * Used for kernel memory, MMIO regions, etc.
     */
    static void reserveFrame(void* frame);
    
    /**
     * @brief Reserve a range of frames
     * @param start Start address
     * @param end End address
     */
    static void reserveRange(uintptr_t start, uintptr_t end);
    
    /**
     * @brief Check if a frame is free
     * @param frame Physical address to check
     * @return true if free, false if allocated/reserved
     */
    static bool isFrameFree(void* frame);
    
    // Statistics
    [[nodiscard]] static size_t getTotalFrames() { return totalFrames_; }
    [[nodiscard]] static size_t getFreeFrames() { return freeFrames_; }
    [[nodiscard]] static size_t getUsedFrames() { return totalFrames_ - freeFrames_; }
    [[nodiscard]] static size_t getTotalMemory() { return totalFrames_ * PAGE_SIZE; }
    [[nodiscard]] static size_t getFreeMemory() { return freeFrames_ * PAGE_SIZE; }
    [[nodiscard]] static size_t getUsedMemory() { return getUsedFrames() * PAGE_SIZE; }
    
    [[nodiscard]] static bool isInitialized() { return initialized_; }

private:
    // Bitmap helpers
    static size_t frameToIndex(size_t frame) { return frame / 32; }
    static size_t frameToBit(size_t frame) { return frame % 32; }
    
    static void markFrame(size_t frameNum);
    static void clearFrame(size_t frameNum);
    static bool testFrame(size_t frameNum);
    
    static size_t findFirstFreeFrame();
    static size_t findContiguousFreeFrames(size_t count);

    static uint32_t* bitmap_;        // Bitmap array (always 32-bit elements)
    static size_t totalFrames_;      // Total number of frames
    static size_t freeFrames_;       // Number of free frames
    static size_t bitmapSize_;       // Size of bitmap in uint32_t's
    static bool initialized_;
};

/**
 * @brief Simple bump allocator for early boot
 * 
 * Used before PhysicalMemory is initialized.
 * Memory allocated this way cannot be freed.
 * 
 * @param size Bytes to allocate
 * @return Pointer to allocated memory
 */
void* kmalloc_early(size_t size);

/**
 * @brief Aligned bump allocator for early boot
 * @param size Bytes to allocate
 * @param align Alignment (must be power of 2)
 * @return Aligned pointer to allocated memory
 */
void* kmalloc_early_aligned(size_t size, size_t align);

/**
 * @brief Get current placement address
 */
uintptr_t getPlacementAddress();

} // namespace myOS::kernel

