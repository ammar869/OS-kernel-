#pragma once

/**
 * @file test_utils.h
 * @brief Test utilities and helper functions for Mini OS Kernel Simulator tests.
 *
 * Provides factory helpers for creating kernel objects in a known state,
 * making individual test cases shorter and more readable.
 */

#include "ProcessState.h"
#include "MemoryManager.h"
#include "SyncManager.h"
#include "ProcessManager.h"
#include "Scheduler.h"

#include <vector>
#include <memory>

// ─────────────────────────────────────────────────────────────────────────────
// ProcessControlBlock helpers
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Create a ProcessControlBlock with sensible defaults.
 *
 * @param pid        Process identifier (must be > 0)
 * @param priority   Priority in [1, 10]
 * @param burst      CPU burst time (> 0)
 * @param arrival    Arrival time (>= 0)
 * @return           Fully initialised PCB in NEW state
 */
inline ProcessControlBlock makeProcess(int pid, int priority = 5,
                                       int burst = 10, int arrival = 0)
{
    ProcessControlBlock pcb(pid, priority, burst, arrival);
    return pcb;
}

/**
 * @brief Create a ProcessControlBlock already in READY state.
 */
inline ProcessControlBlock makeReadyProcess(int pid, int priority = 5,
                                            int burst = 10, int arrival = 0)
{
    ProcessControlBlock pcb(pid, priority, burst, arrival);
    pcb.state = ProcessState::READY;
    return pcb;
}

// ─────────────────────────────────────────────────────────────────────────────
// ProcessManager helpers
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Populate a ProcessManager with @p n processes and move them to READY.
 *
 * Processes are created with priority cycling 1-10, burst time = 10,
 * and arrival time equal to their index.
 *
 * @param pm  ProcessManager to populate (will be reset first)
 * @param n   Number of processes to create
 * @return    Vector of PIDs in creation order
 */
inline std::vector<int> buildReadyQueue(ProcessManager& pm, int n)
{
    pm.reset();
    std::vector<int> pids;
    pids.reserve(n);
    for (int i = 0; i < n; ++i) {
        int priority = (i % 10) + 1;   // 1-10
        int pid = pm.createProcess(priority, 10, i);
        if (pid > 0) {
            pm.updateProcessState(pid, ProcessState::READY);
            pids.push_back(pid);
        }
    }
    return pids;
}

/**
 * @brief Return raw pointers to all READY processes in @p pm.
 *
 * Convenience wrapper around ProcessManager::getProcessesByState.
 */
inline std::vector<ProcessControlBlock*> getReadyProcesses(ProcessManager& pm)
{
    return pm.getProcessesByState(ProcessState::READY);
}

// ─────────────────────────────────────────────────────────────────────────────
// MemoryManager helpers
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Create a MemoryManager with a given number of frames.
 *
 * Uses a page size of 64 bytes so total_memory = frame_count * 64.
 *
 * @param frame_count  Number of page frames
 * @param algo         Replacement algorithm (default LRU)
 * @return             Configured MemoryManager
 */
inline MemoryManager makeMemoryManager(
    int frame_count,
    ReplacementAlgorithm algo = ReplacementAlgorithm::LRU)
{
    const int page_size = 64;
    MemoryConfiguration cfg(frame_count * page_size, page_size, algo);
    return MemoryManager(cfg);
}

// ─────────────────────────────────────────────────────────────────────────────
// SyncManager helpers
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Create a SyncManager pre-populated with semaphores and mutexes.
 *
 * Returns a heap-allocated SyncManager (SyncManager is non-copyable due to
 * internal std::mutex members).
 *
 * @param sem_count    Number of semaphores to create (initial count = 1 each)
 * @param mutex_count  Number of mutexes to create
 * @param sem_ids      [out] IDs of created semaphores
 * @param mutex_ids    [out] IDs of created mutexes
 * @return             unique_ptr to configured SyncManager
 */
inline std::unique_ptr<SyncManager> makeSyncManager(int sem_count, int mutex_count,
                                                    std::vector<int>& sem_ids,
                                                    std::vector<int>& mutex_ids)
{
    auto sm = std::make_unique<SyncManager>();
    sem_ids.clear();
    mutex_ids.clear();

    for (int i = 0; i < sem_count; ++i) {
        int id = sm->createSemaphore(1);
        if (id > 0) sem_ids.push_back(id);
    }
    for (int i = 0; i < mutex_count; ++i) {
        int id = sm->createMutex();
        if (id > 0) mutex_ids.push_back(id);
    }
    return sm;
}

// ─────────────────────────────────────────────────────────────────────────────
// Parameter constraint helpers (useful in property generators)
// ─────────────────────────────────────────────────────────────────────────────

/// Returns true if the given process parameters are valid per InputValidator rules.
inline bool isValidProcessParams(int priority, int burst, int arrival)
{
    return priority >= 1 && priority <= 10 && burst > 0 && arrival >= 0;
}

/// Returns true if the memory configuration is valid (page_size must be power-of-2).
inline bool isValidMemoryConfig(int total_size, int page_size)
{
    if (page_size <= 0 || total_size <= 0) return false;
    if (total_size % page_size != 0) return false;
    // page_size must be a power of two
    return (page_size & (page_size - 1)) == 0;
}
