#pragma once

#include <unordered_map>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * @brief Manages synchronization primitives for process coordination
 * 
 * The SyncManager provides semaphores and mutexes for process synchronization.
 * It handles process blocking, waking, and deadlock prevention.
 */
class SyncManager {
public:
    /**
     * @brief Semaphore synchronization primitive
     */
    class Semaphore {
    public:
        /**
         * @brief Constructor
         * @param initial_count Initial semaphore count
         */
        explicit Semaphore(int initial_count = 1);
        
        /**
         * @brief Destructor
         */
        ~Semaphore() = default;
        
        /**
         * @brief Wait operation (P operation)
         * @param process_id Process requesting the semaphore
         * @return true if acquired immediately, false if process should be blocked
         */
        bool wait(int process_id);
        
        /**
         * @brief Signal operation (V operation)
         * @return Process ID that was woken up, or -1 if no process was waiting
         */
        int signal();
        
        /**
         * @brief Get current semaphore count
         * @return Current count
         */
        int getCount() const;
        
        /**
         * @brief Get number of waiting processes
         * @return Number of processes in waiting queue
         */
        size_t getWaitingCount() const;
        
        /**
         * @brief Check if a process is waiting on this semaphore
         * @param process_id Process ID to check
         * @return true if process is waiting
         */
        bool isProcessWaiting(int process_id) const;
        
        /**
         * @brief Reset semaphore to initial state
         * @param initial_count New initial count
         */
        void reset(int initial_count = 1);

    private:
        int count_;
        std::queue<int> waiting_processes_;
        mutable std::mutex sem_mutex_;
    };
    
    /**
     * @brief Mutex synchronization primitive
     */
    class Mutex {
    public:
        /**
         * @brief Constructor
         */
        Mutex();
        
        /**
         * @brief Destructor
         */
        ~Mutex() = default;
        
        /**
         * @brief Lock operation
         * @param process_id Process requesting the lock
         * @return true if acquired immediately, false if process should be blocked
         */
        bool lock(int process_id);
        
        /**
         * @brief Unlock operation
         * @param process_id Process releasing the lock
         * @return Process ID that was woken up, or -1 if no process was waiting or wrong owner
         */
        int unlock(int process_id);
        
        /**
         * @brief Check if mutex is locked
         * @return true if locked
         */
        bool isLocked() const;
        
        /**
         * @brief Get owner process ID
         * @return Process ID of owner, or -1 if not locked
         */
        int getOwner() const;
        
        /**
         * @brief Get number of waiting processes
         * @return Number of processes in waiting queue
         */
        size_t getWaitingCount() const;
        
        /**
         * @brief Check if a process is waiting on this mutex
         * @param process_id Process ID to check
         * @return true if process is waiting
         */
        bool isProcessWaiting(int process_id) const;
        
        /**
         * @brief Reset mutex to unlocked state
         */
        void reset();

    private:
        int owner_process_;
        std::queue<int> waiting_processes_;
        mutable std::mutex mtx_mutex_;
    };

public:
    /**
     * @brief Constructor
     */
    SyncManager();
    
    /**
     * @brief Destructor
     */
    ~SyncManager();
    
    /**
     * @brief Create a new semaphore
     * @param initial_count Initial semaphore count
     * @return Semaphore ID, or -1 if creation failed
     */
    int createSemaphore(int initial_count = 1);
    
    /**
     * @brief Create a new mutex
     * @return Mutex ID, or -1 if creation failed
     */
    int createMutex();
    
    /**
     * @brief Get semaphore by ID
     * @param sem_id Semaphore ID
     * @return Pointer to semaphore, or nullptr if not found
     */
    Semaphore* getSemaphore(int sem_id);
    
    /**
     * @brief Get mutex by ID
     * @param mutex_id Mutex ID
     * @return Pointer to mutex, or nullptr if not found
     */
    Mutex* getMutex(int mutex_id);
    
    /**
     * @brief Remove a semaphore
     * @param sem_id Semaphore ID to remove
     * @return true if removed successfully
     */
    bool removeSemaphore(int sem_id);
    
    /**
     * @brief Remove a mutex
     * @param mutex_id Mutex ID to remove
     * @return true if removed successfully
     */
    bool removeMutex(int mutex_id);
    
    /**
     * @brief Get all processes waiting on any synchronization primitive
     * @return Vector of process IDs that are blocked
     */
    std::vector<int> getAllWaitingProcesses() const;
    
    /**
     * @brief Check if a process is waiting on any synchronization primitive
     * @param process_id Process ID to check
     * @return true if process is waiting
     */
    bool isProcessWaiting(int process_id) const;
    
    /**
     * @brief Reset all synchronization primitives
     */
    void reset();
    
    /**
     * @brief Get total number of semaphores
     * @return Semaphore count
     */
    size_t getSemaphoreCount() const;
    
    /**
     * @brief Get total number of mutexes
     * @return Mutex count
     */
    size_t getMutexCount() const;
    
    /**
     * @brief Detect potential deadlocks
     * @return true if deadlock detected
     */
    bool detectDeadlock() const;
    
    /**
     * @brief Clean up resources for a terminated process
     * @param process_id Process ID that terminated
     */
    void cleanupProcess(int process_id);

private:
    std::unordered_map<int, std::unique_ptr<Semaphore>> semaphores_;
    std::unordered_map<int, std::unique_ptr<Mutex>> mutexes_;
    int next_sem_id_;
    int next_mutex_id_;
    mutable std::mutex sync_mutex_;
    
    /**
     * @brief Generate next semaphore ID
     * @return Next available semaphore ID
     */
    int generateNextSemaphoreID();
    
    /**
     * @brief Generate next mutex ID
     * @return Next available mutex ID
     */
    int generateNextMutexID();
};