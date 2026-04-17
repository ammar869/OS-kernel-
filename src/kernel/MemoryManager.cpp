#include "MemoryManager.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

// ============================================================================
// MemoryManager Implementation
// ============================================================================

MemoryManager::MemoryManager(const MemoryConfiguration& config)
    : config_(config), page_fault_count_(0), page_replacement_count_(0), current_time_(0) {
    
    initializeFrames();
    setReplacementAlgorithm(config_.replacement_algo);
}

MemoryManager::~MemoryManager() {
    reset();
}

bool MemoryManager::allocatePages(int process_id, int num_pages) {
    std::lock_guard<std::mutex> lock(memory_mutex_);
    
    if (num_pages <= 0) {
        return false;
    }
    
    // Check if we have enough free frames
    int free_frames = getFreeFrameCount();
    if (free_frames < num_pages) {
        return false;
    }
    
    // Create page table for process if it doesn't exist
    if (page_tables_.find(process_id) == page_tables_.end()) {
        page_tables_[process_id] = std::vector<PageTableEntry>();
    }
    
    // Allocate pages
    auto& page_table = page_tables_[process_id];
    int allocated_pages = 0;
    
    for (int virtual_page = 0; virtual_page < num_pages; ++virtual_page) {
        int frame_id = findFreeFrame();
        if (frame_id == -1) {
            // This shouldn't happen since we checked free frames above
            break;
        }
        
        // Allocate the frame
        page_frames_[frame_id].process_id = process_id;
        page_frames_[frame_id].page_number = virtual_page;
        page_frames_[frame_id].is_allocated = true;
        page_frames_[frame_id].allocation_time = current_time_;
        page_frames_[frame_id].last_access_time = current_time_;
        
        // Update page table
        PageTableEntry entry(virtual_page);
        entry.physical_frame = frame_id;
        entry.is_valid = true;
        page_table.push_back(entry);
        
        allocated_pages++;
    }
    
    return allocated_pages == num_pages;
}

void MemoryManager::deallocatePages(int process_id) {
    std::lock_guard<std::mutex> lock(memory_mutex_);
    
    // Find all frames belonging to this process
    for (auto& frame : page_frames_) {
        if (frame.process_id == process_id && frame.is_allocated) {
            frame.process_id = -1;
            frame.page_number = -1;
            frame.is_allocated = false;
            frame.last_access_time = 0;
            frame.allocation_time = 0;
        }
    }
    
    // Remove page table
    removeProcessFromPageTable(process_id);
}

int MemoryManager::handlePageFault(int process_id, int virtual_page) {
    std::lock_guard<std::mutex> lock(memory_mutex_);
    
    page_fault_count_++;
    
    // Try to find a free frame first
    int frame_id = findFreeFrame();
    
    if (frame_id == -1) {
        // No free frames, need to replace a page
        frame_id = selectVictimFrame();
        if (frame_id == -1) {
            return -1; // No frames available
        }
        
        // Remove old mapping
        PageFrame& victim_frame = page_frames_[frame_id];
        if (victim_frame.is_allocated) {
            updatePageTableEntry(victim_frame.process_id, victim_frame.page_number, -1, false);
        }
        page_replacement_count_++;
    }
    
    // Allocate the frame to the new page
    page_frames_[frame_id].process_id = process_id;
    page_frames_[frame_id].page_number = virtual_page;
    page_frames_[frame_id].is_allocated = true;
    page_frames_[frame_id].allocation_time = current_time_;
    page_frames_[frame_id].last_access_time = current_time_;
    
    // Update page table
    updatePageTableEntry(process_id, virtual_page, frame_id, true);
    
    return frame_id;
}

void MemoryManager::setReplacementAlgorithm(ReplacementAlgorithm algorithm) {
    std::lock_guard<std::mutex> lock(memory_mutex_);
    config_.replacement_algo = algorithm;
    replacement_algorithm_ = createReplacementAlgorithm(algorithm);
}

const std::vector<PageTableEntry>* MemoryManager::getPageTable(int process_id) const {
    std::lock_guard<std::mutex> lock(memory_mutex_);
    
    auto it = page_tables_.find(process_id);
    if (it == page_tables_.end()) {
        return nullptr;
    }
    
    return &it->second;
}

int MemoryManager::getFreeFrameCount() const {
    int count = 0;
    for (const auto& frame : page_frames_) {
        if (!frame.is_allocated) {
            count++;
        }
    }
    return count;
}

double MemoryManager::getMemoryUtilization() const {
    std::lock_guard<std::mutex> lock(memory_mutex_);
    
    int allocated_frames = config_.num_frames - getFreeFrameCount();
    return (static_cast<double>(allocated_frames) / config_.num_frames) * 100.0;
}

void MemoryManager::reset() {
    std::lock_guard<std::mutex> lock(memory_mutex_);
    
    // Reset all frames
    for (auto& frame : page_frames_) {
        frame.process_id = -1;
        frame.page_number = -1;
        frame.is_allocated = false;
        frame.last_access_time = 0;
        frame.allocation_time = 0;
    }
    
    // Clear page tables
    page_tables_.clear();
    
    // Reset statistics
    page_fault_count_ = 0;
    page_replacement_count_ = 0;
    current_time_ = 0;
    
    // Reset replacement algorithm
    if (replacement_algorithm_) {
        replacement_algorithm_->reset();
    }
}

void MemoryManager::accessPage(int process_id, int virtual_page, int current_time) {
    std::lock_guard<std::mutex> lock(memory_mutex_);
    
    current_time_ = current_time;
    
    // Find the physical frame for this virtual page
    int frame_id = getPhysicalFrame(process_id, virtual_page);
    if (frame_id != -1) {
        page_frames_[frame_id].last_access_time = current_time;
        
        // Notify replacement algorithm
        if (replacement_algorithm_) {
            replacement_algorithm_->onPageAccess(frame_id, current_time);
        }
    }
}

bool MemoryManager::isPageInMemory(int process_id, int virtual_page) const {
    return getPhysicalFrame(process_id, virtual_page) != -1;
}

int MemoryManager::getPhysicalFrame(int process_id, int virtual_page) const {
    auto it = page_tables_.find(process_id);
    if (it == page_tables_.end()) {
        return -1;
    }
    
    const auto& page_table = it->second;
    for (const auto& entry : page_table) {
        if (entry.virtual_page == virtual_page && entry.is_valid) {
            return entry.physical_frame;
        }
    }
    
    return -1;
}

int MemoryManager::findFreeFrame() {
    for (int i = 0; i < static_cast<int>(page_frames_.size()); ++i) {
        if (!page_frames_[i].is_allocated) {
            return i;
        }
    }
    return -1;
}

int MemoryManager::selectVictimFrame() {
    if (!replacement_algorithm_) {
        return -1;
    }
    
    return replacement_algorithm_->selectVictimFrame(page_frames_);
}

void MemoryManager::initializeFrames() {
    page_frames_.clear();
    page_frames_.reserve(config_.num_frames);
    
    for (int i = 0; i < config_.num_frames; ++i) {
        page_frames_.emplace_back(i);
    }
}

std::unique_ptr<PageReplacementAlgorithm> MemoryManager::createReplacementAlgorithm(ReplacementAlgorithm algorithm) {
    switch (algorithm) {
        case ReplacementAlgorithm::FIFO:
            return std::make_unique<FIFOReplacementAlgorithm>();
        case ReplacementAlgorithm::LRU:
            return std::make_unique<LRUReplacementAlgorithm>();
        default:
            return std::make_unique<LRUReplacementAlgorithm>();
    }
}

void MemoryManager::updatePageTableEntry(int process_id, int virtual_page, int physical_frame, bool is_valid) {
    // Ensure page table exists
    if (page_tables_.find(process_id) == page_tables_.end()) {
        page_tables_[process_id] = std::vector<PageTableEntry>();
    }
    
    auto& page_table = page_tables_[process_id];
    
    // Find existing entry or create new one
    bool found = false;
    for (auto& entry : page_table) {
        if (entry.virtual_page == virtual_page) {
            entry.physical_frame = physical_frame;
            entry.is_valid = is_valid;
            found = true;
            break;
        }
    }
    
    if (!found && is_valid) {
        PageTableEntry entry(virtual_page);
        entry.physical_frame = physical_frame;
        entry.is_valid = is_valid;
        page_table.push_back(entry);
    }
}

void MemoryManager::removeProcessFromPageTable(int process_id) {
    page_tables_.erase(process_id);
}

// ============================================================================
// FIFO Page Replacement Algorithm Implementation
// ============================================================================

int FIFOReplacementAlgorithm::selectVictimFrame(const std::vector<PageFrame>& frames) {
    int oldest_time = std::numeric_limits<int>::max();
    int victim_frame = -1;
    
    for (int i = 0; i < static_cast<int>(frames.size()); ++i) {
        if (frames[i].is_allocated && frames[i].allocation_time < oldest_time) {
            oldest_time = frames[i].allocation_time;
            victim_frame = i;
        }
    }
    
    return victim_frame;
}

// ============================================================================
// LRU Page Replacement Algorithm Implementation
// ============================================================================

int LRUReplacementAlgorithm::selectVictimFrame(const std::vector<PageFrame>& frames) {
    int least_recent_time = std::numeric_limits<int>::max();
    int victim_frame = -1;
    
    for (int i = 0; i < static_cast<int>(frames.size()); ++i) {
        if (frames[i].is_allocated && frames[i].last_access_time < least_recent_time) {
            least_recent_time = frames[i].last_access_time;
            victim_frame = i;
        }
    }
    
    return victim_frame;
}

void LRUReplacementAlgorithm::onPageAccess(int frame_id, int current_time) {
    // The MemoryManager handles updating the last_access_time
    // This method is called for notification purposes
}