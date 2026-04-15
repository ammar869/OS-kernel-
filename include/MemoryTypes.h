#pragma once

#include <vector>

/**
 * @brief Page replacement algorithms supported by the memory manager
 */
enum class ReplacementAlgorithm {
    FIFO,  ///< First-In-First-Out page replacement
    LRU    ///< Least Recently Used page replacement
};

/**
 * @brief CPU scheduling algorithms supported by the scheduler
 */
enum class SchedulingAlgorithm {
    FCFS,        ///< First-Come-First-Served
    ROUND_ROBIN, ///< Round Robin with time quantum
    PRIORITY     ///< Priority-based scheduling
};

/**
 * @brief Memory configuration parameters
 */
struct MemoryConfiguration {
    int total_memory_size;           ///< Total memory size in bytes
    int page_size;                   ///< Size of each page in bytes
    int num_frames;                  ///< Number of page frames
    ReplacementAlgorithm replacement_algo;  ///< Page replacement algorithm
    
    MemoryConfiguration(int total_size = 1024, int page_sz = 64, 
                       ReplacementAlgorithm algo = ReplacementAlgorithm::LRU)
        : total_memory_size(total_size), page_size(page_sz),
          num_frames(total_size / page_sz), replacement_algo(algo) {}
};

/**
 * @brief Page frame structure for memory management
 */
struct PageFrame {
    int frame_id;           ///< Frame identifier
    int process_id;         ///< ID of process owning this frame (-1 if free)
    int page_number;        ///< Virtual page number mapped to this frame
    bool is_allocated;      ///< Whether frame is currently allocated
    int last_access_time;   ///< Last time this frame was accessed (for LRU)
    int allocation_time;    ///< Time when frame was allocated (for FIFO)
    
    PageFrame(int id) : frame_id(id), process_id(-1), page_number(-1),
                       is_allocated(false), last_access_time(0), allocation_time(0) {}
};

/**
 * @brief Page table entry for virtual-to-physical address translation
 */
struct PageTableEntry {
    int virtual_page;       ///< Virtual page number
    int physical_frame;     ///< Physical frame number (-1 if not in memory)
    bool is_valid;          ///< Whether page is currently in memory
    bool is_dirty;          ///< Whether page has been modified
    
    PageTableEntry(int vpage = -1) : virtual_page(vpage), physical_frame(-1),
                                    is_valid(false), is_dirty(false) {}
};