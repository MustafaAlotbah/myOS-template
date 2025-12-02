/**
 * @file PhysicalMemory.cpp
 * @brief Physical memory manager implementation
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <memory.h>
#include <myOS/kernel/Logger.h>
#include <myOS/kernel/memory/PhysicalMemory.h>


// Linker symbol - end of kernel in memory
extern "C" char __kernel_end[];

namespace myOS::kernel {

    // ============================================================================
    // STATIC MEMBERS
    // ============================================================================

    uint32_t* PhysicalMemory::bitmap_   = nullptr;
    size_t PhysicalMemory::totalFrames_ = 0;
    size_t PhysicalMemory::freeFrames_  = 0;
    size_t PhysicalMemory::bitmapSize_  = 0;
    bool PhysicalMemory::initialized_   = false;

    // Placement address for early allocations
    static uintptr_t s_placementAddress = 0;

    // ============================================================================
    // EARLY BOOT ALLOCATOR
    // ============================================================================

    void* kmalloc_early(size_t size) {
        if (size == 0) return nullptr;

        // Initialize placement address if needed
        if (s_placementAddress == 0) { s_placementAddress = reinterpret_cast<uintptr_t>(__kernel_end); }

        uintptr_t addr = s_placementAddress;
        s_placementAddress += size;
        return reinterpret_cast<void*>(addr);
    }

    void* kmalloc_early_aligned(size_t size, size_t align) {
        if (size == 0) return nullptr;

        // Initialize placement address if needed
        if (s_placementAddress == 0) { s_placementAddress = reinterpret_cast<uintptr_t>(__kernel_end); }

        // Align the address
        if (s_placementAddress & (align - 1)) { s_placementAddress = (s_placementAddress + align) & ~(align - 1); }

        uintptr_t addr = s_placementAddress;
        s_placementAddress += size;
        return reinterpret_cast<void*>(addr);
    }

    uintptr_t getPlacementAddress() {
        if (s_placementAddress == 0) { s_placementAddress = reinterpret_cast<uintptr_t>(__kernel_end); }
        return s_placementAddress;
    }

    // ============================================================================
    // BITMAP OPERATIONS
    // ============================================================================

    void PhysicalMemory::markFrame(size_t frameNum) {
        size_t idx = frameToIndex(frameNum);
        size_t bit = frameToBit(frameNum);
        bitmap_[idx] |= (1U << bit);
    }

    void PhysicalMemory::clearFrame(size_t frameNum) {
        size_t idx = frameToIndex(frameNum);
        size_t bit = frameToBit(frameNum);
        bitmap_[idx] &= ~(1U << bit);
    }

    bool PhysicalMemory::testFrame(size_t frameNum) {
        size_t idx = frameToIndex(frameNum);
        size_t bit = frameToBit(frameNum);
        return (bitmap_[idx] & (1U << bit)) != 0;
    }

    size_t PhysicalMemory::findFirstFreeFrame() {
        for (size_t i = 0; i < bitmapSize_; ++i) {
            // Skip fully used blocks
            if (bitmap_[i] == 0xFFFFFFFF) continue;

            // Find free bit in this block
            for (size_t j = 0; j < 32; ++j) {
                if (!(bitmap_[i] & (1U << j))) { return i * 32 + j; }
            }
        }
        return SIZE_MAX;  // No free frame
    }

    size_t PhysicalMemory::findContiguousFreeFrames(size_t count) {
        size_t consecutive = 0;
        size_t startFrame  = 0;

        for (size_t frame = 0; frame < totalFrames_; ++frame) {
            if (!testFrame(frame)) {
                if (consecutive == 0) startFrame = frame;
                consecutive++;
                if (consecutive == count) return startFrame;
            }
            else { consecutive = 0; }
        }
        return SIZE_MAX;  // Not enough contiguous frames
    }

    // ============================================================================
    // PUBLIC API
    // ============================================================================

    void PhysicalMemory::initialize(uintptr_t kernelEnd, size_t memorySize) {
        LOG_INFO("Initializing physical memory manager");
        LOG_DEBUG("  Kernel end: 0x%x", static_cast<uint32_t>(kernelEnd));
        LOG_DEBUG("  Memory size: %u MB", static_cast<uint32_t>(memorySize / (1024 * 1024)));

        // Calculate number of frames
        totalFrames_ = memorySize / PAGE_SIZE;
        bitmapSize_  = (totalFrames_ + 31) / 32;  // Round up

        LOG_DEBUG("  Total frames: %u", static_cast<uint32_t>(totalFrames_));
        LOG_DEBUG("  Bitmap size: %u bytes", static_cast<uint32_t>(bitmapSize_ * 4));

        // Allocate bitmap using early allocator
        bitmap_ = static_cast<uint32_t*>(kmalloc_early(bitmapSize_ * sizeof(uint32_t)));

        // Clear bitmap (all frames free)
        memset(bitmap_, 0, bitmapSize_ * sizeof(uint32_t));
        freeFrames_ = totalFrames_;

        // Reserve low memory (0 - 1MB) - BIOS, VGA, etc.
        reserveRange(0, 0x100000);

        // Reserve kernel + our bitmap
        uintptr_t reserveEnd = getPlacementAddress();
        reserveRange(0x100000, reserveEnd);  // Kernel starts at 1MB typically

        LOG_DEBUG("  Reserved up to: 0x%x", static_cast<uint32_t>(reserveEnd));
        LOG_INFO("Physical memory: %u MB total, %u MB free", static_cast<uint32_t>(getTotalMemory() / (1024 * 1024)), static_cast<uint32_t>(getFreeMemory() / (1024 * 1024)));

        initialized_ = true;
    }

    void* PhysicalMemory::allocateFrame() {
        if (!initialized_) return nullptr;
        if (freeFrames_ == 0) return nullptr;

        size_t frame = findFirstFreeFrame();
        if (frame == SIZE_MAX) return nullptr;

        markFrame(frame);
        freeFrames_--;

        return reinterpret_cast<void*>(static_cast<uintptr_t>(frame * PAGE_SIZE));
    }

    void* PhysicalMemory::allocateFrames(size_t count) {
        if (!initialized_) return nullptr;
        if (count == 0) return nullptr;
        if (freeFrames_ < count) return nullptr;

        size_t startFrame = findContiguousFreeFrames(count);
        if (startFrame == SIZE_MAX) return nullptr;

        for (size_t i = 0; i < count; ++i) { markFrame(startFrame + i); }
        freeFrames_ -= count;

        return reinterpret_cast<void*>(static_cast<uintptr_t>(startFrame * PAGE_SIZE));
    }

    void PhysicalMemory::freeFrame(void* frame) {
        if (!initialized_) return;
        if (frame == nullptr) return;

        size_t frameNum = reinterpret_cast<uintptr_t>(frame) / PAGE_SIZE;
        if (frameNum >= totalFrames_) return;

        // Only free if actually allocated
        if (testFrame(frameNum)) {
            clearFrame(frameNum);
            freeFrames_++;
        }
    }

    void PhysicalMemory::freeFrames(void* frame, size_t count) {
        if (!initialized_) return;
        if (frame == nullptr || count == 0) return;

        size_t startFrame = reinterpret_cast<uintptr_t>(frame) / PAGE_SIZE;

        for (size_t i = 0; i < count; ++i) {
            size_t frameNum = startFrame + i;
            if (frameNum < totalFrames_ && testFrame(frameNum)) {
                clearFrame(frameNum);
                freeFrames_++;
            }
        }
    }

    void PhysicalMemory::reserveFrame(void* frame) {
        if (!initialized_) return;

        size_t frameNum = reinterpret_cast<uintptr_t>(frame) / PAGE_SIZE;
        if (frameNum >= totalFrames_) return;

        if (!testFrame(frameNum)) {
            markFrame(frameNum);
            freeFrames_--;
        }
    }

    void PhysicalMemory::reserveRange(uintptr_t start, uintptr_t end) {
        // Align to page boundaries
        size_t startFrame = start / PAGE_SIZE;
        size_t endFrame   = (end + PAGE_SIZE - 1) / PAGE_SIZE;

        for (size_t frame = startFrame; frame < endFrame && frame < totalFrames_; ++frame) {
            if (!testFrame(frame)) {
                markFrame(frame);
                freeFrames_--;
            }
        }
    }

    bool PhysicalMemory::isFrameFree(void* frame) {
        if (!initialized_) return false;

        size_t frameNum = reinterpret_cast<uintptr_t>(frame) / PAGE_SIZE;
        if (frameNum >= totalFrames_) return false;

        return !testFrame(frameNum);
    }

}  // namespace myOS::kernel
