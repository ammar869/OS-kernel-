#include "ProcessManager.h"
#include <algorithm>
#include <stdexcept>

ProcessManager::ProcessManager() : next_pid_(1) {
}

ProcessManager::~ProcessManager() {
    reset();
}

int ProcessManager::createProcess(int priority, int burst_time, int arrival_time) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    
    // Validate parameters
    if (!validateProcessParameters(priority, burst_time, arrival_time)) {
        return -1;
    }
    
    // Generate new PID
    int pid = generateNextPID();
    
    // Create new process control block
    auto pcb = std::make_unique<ProcessControlBlock>(pid, priority, burst_time, arrival_time);
    
    // Store the process
    processes_[pid] = std::move(pcb);
    
    return pid;
}

bool ProcessManager::terminateProcess(int pid) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    
    auto it = processes_.find(pid);
    if (it == processes_.end()) {
        return false;
    }
    
    // Update state to terminated before cleanup
    it->second->state = ProcessState::TERMINATED;
    
    // Clean up allocated pages (will be handled by memory manager)
    it->second->allocated_pages.clear();
    
    // Remove from processes map
    processes_.erase(it);
    
    return true;
}

bool ProcessManager::updateProcessState(int pid, ProcessState new_state) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    
    auto it = processes_.find(pid);
    if (it == processes_.end()) {
        return false;
    }
    
    // Validate state transition (basic validation)
    ProcessState current_state = it->second->state;
    
    // Allow any transition for now - more sophisticated validation can be added
    it->second->state = new_state;
    
    return true;
}

ProcessControlBlock* ProcessManager::getProcess(int pid) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    
    auto it = processes_.find(pid);
    if (it == processes_.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

std::vector<ProcessControlBlock*> ProcessManager::getAllProcesses() {
    std::lock_guard<std::mutex> lock(process_mutex_);
    
    std::vector<ProcessControlBlock*> result;
    result.reserve(processes_.size());
    
    for (const auto& pair : processes_) {
        result.push_back(pair.second.get());
    }
    
    // Sort by PID for consistent ordering
    std::sort(result.begin(), result.end(), 
              [](const ProcessControlBlock* a, const ProcessControlBlock* b) {
                  return a->pid < b->pid;
              });
    
    return result;
}

std::vector<ProcessControlBlock*> ProcessManager::getProcessesByState(ProcessState state) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    
    std::vector<ProcessControlBlock*> result;
    
    for (const auto& pair : processes_) {
        if (pair.second->state == state) {
            result.push_back(pair.second.get());
        }
    }
    
    // Sort by PID for consistent ordering
    std::sort(result.begin(), result.end(), 
              [](const ProcessControlBlock* a, const ProcessControlBlock* b) {
                  return a->pid < b->pid;
              });
    
    return result;
}

size_t ProcessManager::getProcessCount() const {
    std::lock_guard<std::mutex> lock(process_mutex_);
    return processes_.size();
}

size_t ProcessManager::getProcessCountByState(ProcessState state) const {
    std::lock_guard<std::mutex> lock(process_mutex_);
    
    size_t count = 0;
    for (const auto& pair : processes_) {
        if (pair.second->state == state) {
            count++;
        }
    }
    
    return count;
}

bool ProcessManager::processExists(int pid) const {
    std::lock_guard<std::mutex> lock(process_mutex_);
    return processes_.find(pid) != processes_.end();
}

void ProcessManager::reset() {
    std::lock_guard<std::mutex> lock(process_mutex_);
    processes_.clear();
    next_pid_ = 1;
}

void ProcessManager::setProcessStartTime(int pid, int start_time) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    
    auto it = processes_.find(pid);
    if (it != processes_.end()) {
        it->second->start_time = start_time;
    }
}

void ProcessManager::setProcessCompletionTime(int pid, int completion_time) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    
    auto it = processes_.find(pid);
    if (it != processes_.end()) {
        it->second->completion_time = completion_time;
    }
}

bool ProcessManager::decreaseRemainingTime(int pid, int time_units) {
    std::lock_guard<std::mutex> lock(process_mutex_);
    
    auto it = processes_.find(pid);
    if (it == processes_.end()) {
        return false;
    }
    
    it->second->remaining_time -= time_units;
    
    // Ensure remaining time doesn't go negative
    if (it->second->remaining_time < 0) {
        it->second->remaining_time = 0;
    }
    
    return it->second->remaining_time > 0;
}

int ProcessManager::generateNextPID() {
    return next_pid_++;
}

bool ProcessManager::validateProcessParameters(int priority, int burst_time, int arrival_time) const {
    // Priority must be between 1 and 10
    if (priority < 1 || priority > 10) {
        return false;
    }
    
    // Burst time must be positive
    if (burst_time <= 0) {
        return false;
    }
    
    // Arrival time cannot be negative
    if (arrival_time < 0) {
        return false;
    }
    
    return true;
}