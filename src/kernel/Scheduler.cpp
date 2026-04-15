#include "Scheduler.h"
#include <algorithm>
#include <limits>

// ============================================================================
// FCFSScheduler Implementation
// ============================================================================

FCFSScheduler::FCFSScheduler() {
}

int FCFSScheduler::selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) {
    if (ready_queue.empty()) {
        return -1;
    }
    
    // Find process with earliest arrival time
    auto earliest = std::min_element(ready_queue.begin(), ready_queue.end(),
        [](const ProcessControlBlock* a, const ProcessControlBlock* b) {
            return a->arrival_time < b->arrival_time;
        });
    
    return (*earliest)->pid;
}

bool FCFSScheduler::shouldPreempt(int current_pid, int new_pid) {
    // FCFS is non-preemptive
    return false;
}

void FCFSScheduler::onTimeQuantumExpired() {
    // No time quantum in FCFS
}

void FCFSScheduler::reset() {
    // No state to reset in FCFS
}

const char* FCFSScheduler::getAlgorithmName() const {
    return "FCFS";
}

// ============================================================================
// RoundRobinScheduler Implementation
// ============================================================================

RoundRobinScheduler::RoundRobinScheduler(int time_quantum) 
    : time_quantum_(time_quantum), current_quantum_remaining_(time_quantum),
      last_scheduled_index_(-1) {
}

int RoundRobinScheduler::selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) {
    if (ready_queue.empty()) {
        return -1;
    }
    
    // Circular queue implementation
    int next_index = (last_scheduled_index_ + 1) % static_cast<int>(ready_queue.size());
    last_scheduled_index_ = next_index;
    
    // Reset quantum for new process
    current_quantum_remaining_ = time_quantum_;
    
    return ready_queue[next_index]->pid;
}

bool RoundRobinScheduler::shouldPreempt(int current_pid, int new_pid) {
    // Round Robin doesn't preempt on process arrival, only on quantum expiration
    return false;
}

void RoundRobinScheduler::onTimeQuantumExpired() {
    current_quantum_remaining_ = 0;
}

void RoundRobinScheduler::reset() {
    current_quantum_remaining_ = time_quantum_;
    last_scheduled_index_ = -1;
}

const char* RoundRobinScheduler::getAlgorithmName() const {
    return "Round Robin";
}

void RoundRobinScheduler::setTimeQuantum(int quantum) {
    if (quantum > 0) {
        time_quantum_ = quantum;
        current_quantum_remaining_ = quantum;
    }
}

bool RoundRobinScheduler::decreaseQuantum(int time_units) {
    current_quantum_remaining_ -= time_units;
    if (current_quantum_remaining_ < 0) {
        current_quantum_remaining_ = 0;
    }
    return current_quantum_remaining_ > 0;
}

// ============================================================================
// PriorityScheduler Implementation
// ============================================================================

PriorityScheduler::PriorityScheduler() {
}

int PriorityScheduler::selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) {
    if (ready_queue.empty()) {
        return -1;
    }
    
    // Find highest priority process (lower number = higher priority)
    auto highest_priority = std::min_element(ready_queue.begin(), ready_queue.end(),
        [](const ProcessControlBlock* a, const ProcessControlBlock* b) {
            return a->priority < b->priority;
        });
    
    return (*highest_priority)->pid;
}

bool PriorityScheduler::shouldPreempt(int current_pid, int new_pid) {
    // This method would need access to ProcessManager to get process priorities
    // For now, we'll implement a simplified version
    // In a complete implementation, this would be injected or passed as parameters
    
    // Simplified: assume preemption should occur if new process has higher priority
    // This would need to be properly implemented with access to process data
    return false; // Placeholder - needs process manager access
}

void PriorityScheduler::onTimeQuantumExpired() {
    // No time quantum in priority scheduling
}

void PriorityScheduler::reset() {
    // No state to reset in priority scheduling
}

const char* PriorityScheduler::getAlgorithmName() const {
    return "Priority";
}

int PriorityScheduler::getProcessPriority(int pid) const {
    // This would need access to ProcessManager
    // Placeholder implementation
    return -1;
}