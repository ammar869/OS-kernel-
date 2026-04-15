#include "SyncManager.h"
#include <algorithm>
#include <set>

// ============================================================================
// Semaphore Implementation
// ============================================================================

SyncManager::Semaphore::Semaphore(int initial_count) : count_(initial_count) {
    if (count_ < 0) {
        count_ = 0;
    }
}

bool SyncManager::Semaphore::wait(int process_id) {
    std::lock_guard<std::mutex> lock(sem_mutex_);
    
    if (count_ > 0) {
        count_--;
        return true; // Acquired immediately
    } else {
        // Add to waiting queue
        waiting_processes_.push(process_id);
        return false; // Process should be blocked
    }
}

int SyncManager::Semaphore::signal() {
    std::lock_guard<std::mutex> lock(sem_mutex_);
    
    if (!waiting_processes_.empty()) {
        // Wake up a waiting process
        int woken_process = waiting_processes_.front();
        waiting_processes_.pop();
        return woken_process;
    } else {
        // No waiting processes, increment count
        count_++;
        return -1;
    }
}

int SyncManager::Semaphore::getCount() const {
    std::lock_guard<std::mutex> lock(sem_mutex_);
    return count_;
}

size_t SyncManager::Semaphore::getWaitingCount() const {
    std::lock_guard<std::mutex> lock(sem_mutex_);
    return waiting_processes_.size();
}

bool SyncManager::Semaphore::isProcessWaiting(int process_id) const {
    std::lock_guard<std::mutex> lock(sem_mutex_);
    
    std::queue<int> temp_queue = waiting_processes_;
    while (!temp_queue.empty()) {
        if (temp_queue.front() == process_id) {
            return true;
        }
        temp_queue.pop();
    }
    return false;
}

void SyncManager::Semaphore::reset(int initial_count) {
    std::lock_guard<std::mutex> lock(sem_mutex_);
    count_ = initial_count;
    if (count_ < 0) {
        count_ = 0;
    }
    
    // Clear waiting queue
    while (!waiting_processes_.empty()) {
        waiting_processes_.pop();
    }
}

// ============================================================================
// Mutex Implementation
// ============================================================================

SyncManager::Mutex::Mutex() : owner_process_(-1) {
}

bool SyncManager::Mutex::lock(int process_id) {
    std::lock_guard<std::mutex> lock(mtx_mutex_);
    
    if (owner_process_ == -1) {
        // Mutex is free, acquire it
        owner_process_ = process_id;
        return true; // Acquired immediately
    } else if (owner_process_ == process_id) {
        // Process already owns the mutex (recursive lock not supported)
        return true;
    } else {
        // Mutex is owned by another process, add to waiting queue
        waiting_processes_.push(process_id);
        return false; // Process should be blocked
    }
}

int SyncManager::Mutex::unlock(int process_id) {
    std::lock_guard<std::mutex> lock(mtx_mutex_);
    
    if (owner_process_ != process_id) {
        // Process doesn't own the mutex
        return -1;
    }
    
    owner_process_ = -1;
    
    if (!waiting_processes_.empty()) {
        // Wake up next waiting process and give it ownership
        int woken_process = waiting_processes_.front();
        waiting_processes_.pop();
        owner_process_ = woken_process;
        return woken_process;
    }
    
    return -1; // No process was waiting
}

bool SyncManager::Mutex::isLocked() const {
    std::lock_guard<std::mutex> lock(mtx_mutex_);
    return owner_process_ != -1;
}

int SyncManager::Mutex::getOwner() const {
    std::lock_guard<std::mutex> lock(mtx_mutex_);
    return owner_process_;
}

size_t SyncManager::Mutex::getWaitingCount() const {
    std::lock_guard<std::mutex> lock(mtx_mutex_);
    return waiting_processes_.size();
}

bool SyncManager::Mutex::isProcessWaiting(int process_id) const {
    std::lock_guard<std::mutex> lock(mtx_mutex_);
    
    std::queue<int> temp_queue = waiting_processes_;
    while (!temp_queue.empty()) {
        if (temp_queue.front() == process_id) {
            return true;
        }
        temp_queue.pop();
    }
    return false;
}

void SyncManager::Mutex::reset() {
    std::lock_guard<std::mutex> lock(mtx_mutex_);
    owner_process_ = -1;
    
    // Clear waiting queue
    while (!waiting_processes_.empty()) {
        waiting_processes_.pop();
    }
}

// ============================================================================
// SyncManager Implementation
// ============================================================================

SyncManager::SyncManager() : next_sem_id_(1), next_mutex_id_(1) {
}

SyncManager::~SyncManager() {
    reset();
}

int SyncManager::createSemaphore(int initial_count) {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    int sem_id = generateNextSemaphoreID();
    semaphores_[sem_id] = std::make_unique<Semaphore>(initial_count);
    
    return sem_id;
}

int SyncManager::createMutex() {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    int mutex_id = generateNextMutexID();
    mutexes_[mutex_id] = std::make_unique<Mutex>();
    
    return mutex_id;
}

SyncManager::Semaphore* SyncManager::getSemaphore(int sem_id) {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    auto it = semaphores_.find(sem_id);
    if (it == semaphores_.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

SyncManager::Mutex* SyncManager::getMutex(int mutex_id) {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    auto it = mutexes_.find(mutex_id);
    if (it == mutexes_.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

bool SyncManager::removeSemaphore(int sem_id) {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    auto it = semaphores_.find(sem_id);
    if (it == semaphores_.end()) {
        return false;
    }
    
    semaphores_.erase(it);
    return true;
}

bool SyncManager::removeMutex(int mutex_id) {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    auto it = mutexes_.find(mutex_id);
    if (it == mutexes_.end()) {
        return false;
    }
    
    mutexes_.erase(it);
    return true;
}

std::vector<int> SyncManager::getAllWaitingProcesses() const {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    std::set<int> waiting_processes;
    
    // Collect from semaphores
    for (const auto& pair : semaphores_) {
        const auto& sem = pair.second;
        std::queue<int> temp_queue;
        
        // We need to access the private waiting_processes_ queue
        // This is a simplified implementation - in practice, we'd need
        // a method to get waiting processes from the semaphore
        for (int pid = 1; pid <= 100; ++pid) { // Check common PID range
            if (sem->isProcessWaiting(pid)) {
                waiting_processes.insert(pid);
            }
        }
    }
    
    // Collect from mutexes
    for (const auto& pair : mutexes_) {
        const auto& mutex = pair.second;
        
        for (int pid = 1; pid <= 100; ++pid) { // Check common PID range
            if (mutex->isProcessWaiting(pid)) {
                waiting_processes.insert(pid);
            }
        }
    }
    
    return std::vector<int>(waiting_processes.begin(), waiting_processes.end());
}

bool SyncManager::isProcessWaiting(int process_id) const {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    // Check semaphores
    for (const auto& pair : semaphores_) {
        if (pair.second->isProcessWaiting(process_id)) {
            return true;
        }
    }
    
    // Check mutexes
    for (const auto& pair : mutexes_) {
        if (pair.second->isProcessWaiting(process_id)) {
            return true;
        }
    }
    
    return false;
}

void SyncManager::reset() {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    semaphores_.clear();
    mutexes_.clear();
    next_sem_id_ = 1;
    next_mutex_id_ = 1;
}

size_t SyncManager::getSemaphoreCount() const {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    return semaphores_.size();
}

size_t SyncManager::getMutexCount() const {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    return mutexes_.size();
}

bool SyncManager::detectDeadlock() const {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    // Simple deadlock detection: check for circular wait conditions
    // This is a simplified implementation - a full implementation would
    // build a resource allocation graph and detect cycles
    
    std::set<int> waiting_processes;
    std::set<int> holding_processes;
    
    // Collect processes that are waiting and holding resources
    for (const auto& pair : mutexes_) {
        const auto& mutex = pair.second;
        int owner = mutex->getOwner();
        
        if (owner != -1) {
            holding_processes.insert(owner);
            
            // Check if owner is also waiting on something
            if (isProcessWaiting(owner)) {
                waiting_processes.insert(owner);
            }
        }
    }
    
    // If any process is both holding and waiting, potential deadlock
    for (int pid : holding_processes) {
        if (waiting_processes.find(pid) != waiting_processes.end()) {
            return true;
        }
    }
    
    return false;
}

void SyncManager::cleanupProcess(int process_id) {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    
    // Release any mutexes owned by this process
    for (const auto& pair : mutexes_) {
        auto& mutex = pair.second;
        if (mutex->getOwner() == process_id) {
            mutex->unlock(process_id);
        }
    }
    
    // Note: We don't automatically clean up waiting processes from queues
    // as they should be handled by the process manager when the process terminates
}

int SyncManager::generateNextSemaphoreID() {
    return next_sem_id_++;
}

int SyncManager::generateNextMutexID() {
    return next_mutex_id_++;
}