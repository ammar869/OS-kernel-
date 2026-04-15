#pragma once

#include "MemoryTypes.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

// Forward declaration for page replacement algorithms
class PageReplacementAlgorithm;

/**
 * @brief Manages memory allocation using paging system
 * 
 * The MemoryManager implements a paging system with configurable page replacement
 * algorithms (FIFO and LRU). It maintains page frames, page tables, and handles
 * page faults and memory allocation/deallocation.
 */
class MemoryManager {
public:
    /**
     * @brief Constructor
     * @param config Memory configuration parameters
     */
    explicit MemoryManager(const MemoryConfiguration& config = MemoryConfiguration());
    
    /**
     * @brief Destructor
     */
    ~MemoryManager();
    
    /**
     * @brief Allocate memory pages for a process
     * @param process_id Process ID requesting memory
     * @param num_pages Number of pages to allocate
     * @return true if allocation successful, false if insufficient memory
     */
    bool allocatePages(int process_id, int num_pages);
    
    /**
     * @brief Deallocate all pages for a process
     * @param process_id Process ID to deallocate memory for
     */
    void deallocatePages(int process_id);
    
    /**
     * @brief Handle a page fault for a process
     * @param process_id Process ID that caused the fault
     * @param virtual_page Virtual page number that caused the fault
     * @return Physical frame number allocated, or -1 if allocation failed
     */
    int handlePageFault(int process_id, int virtual_page);
    
    /**
     * @brief Set the page replacement algorithm
     * @param algorithm Algorithm to use (FIFO or LRU)
     */
    void setReplacementAlgorithm(ReplacementAlgorithm algorithm);
    
    /**
     * @brief Get current memory configuration
     * @return Memory configuration
     */
    const MemoryConfiguration& getConfiguration() const { return config_; }
    
    /**
     * @brief Get all page frames
     * @return Vector of all page frames
     */
    const std::vector<PageFrame>& getPageFrames() const { return page_frames_; }
    
    /**
     * @brief Get page table for a specific process
     * @param process_id Process ID
     * @return Pointer to page table, or nullptr if process not found
     */
    const std::vector<PageTableEntry>* getPageTable(int process_id) const;
    
    /**
     * @brief Get number of free frames
     * @return Number of unallocated frames
     */
    int getFreeFrameCount() const;
    
    /**
     * @brief Get memory utilization percentage
     * @return Utilization as percentage (0-100)
     */
    double getMemoryUtilization() const;
    
    /**
     * @brief Get total number of page faults
     * @return Page fault count
     */
    int getPageFaultCount() const { return page_fault_count_; }
    
    /**
     * @brief Get total number of page replacements
     * @return Page replacement count
     */
    int getPageReplacementCount() const { return page_replacement_count_; }
    
    /**
     * @brief Reset memory manager state
     */
    void reset();
    
    /**
     * @brief Access a page (for LRU tracking)
     * @param process_id Process ID
     * @param virtual_page Virtual page number
     * @param current_time Current simulation time
     */
    void accessPage(int process_id, int virtual_page, int current_time);
    
    /**
     * @brief Check if a virtual page is in memory
     * @param process_id Process ID
     * @param virtual_page Virtual page number
     * @return true if page is in memory
     */
    bool isPageInMemory(int process_id, int virtual_page) const;
    
    /**
     * @brief Get physical frame for a virtual page
     * @param process_id Process ID
     * @param virtual_page Virtual page number
     * @return Physical frame number, or -1 if not in memory
     */
    int getPhysicalFrame(int process_id, int virtual_page) const;

private:
    MemoryConfiguration config_;
    std::vector<PageFrame> page_frames_;
    std::unordered_map<int, std::vector<PageTableEntry>> page_tables_;
    std::unique_ptr<PageReplacementAlgorithm> replacement_algorithm_;
    mutable std::mutex memory_mutex_;
    
    // Statistics
    int page_fault_count_;
    int page_replacement_count_;
    int current_time_;
    
    /**
     * @brief Find a free frame
     * @return Frame index, or -1 if no free frames
     */
    int findFreeFrame();
    
    /**
     * @brief Select a victim frame for replacement
     * @return Frame index to replace
     */
    int selectVictimFrame();
    
    /**
     * @brief Initialize page frames
     */
    void initializeFrames();
    
    /**
     * @brief Create page replacement algorithm instance
     * @param algorithm Algorithm type
     * @return Unique pointer to algorithm instance
     */
    std::unique_ptr<PageReplacementAlgorithm> createReplacementAlgorithm(ReplacementAlgorithm algorithm);
    
    /**
     * @brief Update page table entry
     * @param process_id Process ID
     * @param virtual_page Virtual page number
     * @param physical_frame Physical frame number
     * @param is_valid Whether page is valid
     */
    void updatePageTableEntry(int process_id, int virtual_page, int physical_frame, bool is_valid);
    
    /**
     * @brief Remove process from page table
     * @param process_id Process ID to remove
     */
    void removeProcessFromPageTable(int process_id);
};

/**
 * @brief Abstract base class for page replacement algorithms
 */
class PageReplacementAlgorithm {
public:
    virtual ~PageReplacementAlgorithm() = default;
    
    /**
     * @brief Select a victim frame for replacement
     * @param frames Vector of all page frames
     * @return Index of frame to replace
     */
    virtual int selectVictimFrame(const std::vector<PageFrame>& frames) = 0;
    
    /**
     * @brief Notify algorithm of page access (for LRU)
     * @param frame_id Frame that was accessed
     * @param current_time Current simulation time
     */
    virtual void onPageAccess(int frame_id, int current_time) {}
    
    /**
     * @brief Reset algorithm state
     */
    virtual void reset() {}
};

/**
 * @brief First-In-First-Out page replacement algorithm
 */
class FIFOReplacementAlgorithm : public PageReplacementAlgorithm {
public:
    int selectVictimFrame(const std::vector<PageFrame>& frames) override;
};

/**
 * @brief Least Recently Used page replacement algorithm
 */
class LRUReplacementAlgorithm : public PageReplacementAlgorithm {
public:
    int selectVictimFrame(const std::vector<PageFrame>& frames) override;
    void onPageAccess(int frame_id, int current_time) override;
};