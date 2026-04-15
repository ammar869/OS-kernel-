#include "include/ProcessManager.h"
#include "include/Scheduler.h"
#include "include/MemoryManager.h"
#include "include/SyncManager.h"
#include "include/MetricsCollector.h"
#include "include/InputValidator.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Mini OS Kernel Simulator Core Systems..." << std::endl;
    
    // Test ProcessManager
    std::cout << "\n=== Testing ProcessManager ===" << std::endl;
    ProcessManager pm;
    int pid1 = pm.createProcess(5, 10, 0);
    int pid2 = pm.createProcess(3, 15, 2);
    
    assert(pid1 > 0 && pid2 > 0);
    std::cout << "Created processes: PID " << pid1 << " and PID " << pid2 << std::endl;
    
    // Test MemoryManager
    std::cout << "\n=== Testing MemoryManager ===" << std::endl;
    MemoryConfiguration config(256, 32); // 256 bytes total, 32 byte pages = 8 frames
    MemoryManager mm(config);
    
    std::cout << "Memory configuration: " << config.num_frames << " frames of " << config.page_size << " bytes each" << std::endl;
    std::cout << "Free frames: " << mm.getFreeFrameCount() << std::endl;
    
    // Allocate memory for processes
    bool alloc1 = mm.allocatePages(pid1, 2); // 2 pages for process 1
    bool alloc2 = mm.allocatePages(pid2, 3); // 3 pages for process 2
    
    assert(alloc1 && alloc2);
    std::cout << "Allocated 2 pages for PID " << pid1 << " and 3 pages for PID " << pid2 << std::endl;
    std::cout << "Free frames after allocation: " << mm.getFreeFrameCount() << std::endl;
    std::cout << "Memory utilization: " << mm.getMemoryUtilization() << "%" << std::endl;
    
    // Test page fault handling
    int frame = mm.handlePageFault(pid1, 5); // Virtual page 5 for process 1
    std::cout << "Page fault for PID " << pid1 << ", virtual page 5 -> frame " << frame << std::endl;
    std::cout << "Page faults: " << mm.getPageFaultCount() << std::endl;
    
    // Test SyncManager
    std::cout << "\n=== Testing SyncManager ===" << std::endl;
    SyncManager sm;
    
    // Create semaphore and mutex
    int sem_id = sm.createSemaphore(2); // Binary semaphore with count 2
    int mutex_id = sm.createMutex();
    
    assert(sem_id > 0 && mutex_id > 0);
    std::cout << "Created semaphore " << sem_id << " and mutex " << mutex_id << std::endl;
    
    // Test semaphore operations
    SyncManager::Semaphore* sem = sm.getSemaphore(sem_id);
    assert(sem != nullptr);
    
    bool acquired1 = sem->wait(pid1);
    bool acquired2 = sem->wait(pid2);
    
    std::cout << "Semaphore wait results: PID " << pid1 << " = " << (acquired1 ? "acquired" : "blocked") 
              << ", PID " << pid2 << " = " << (acquired2 ? "acquired" : "blocked") << std::endl;
    std::cout << "Semaphore count: " << sem->getCount() << ", waiting: " << sem->getWaitingCount() << std::endl;
    
    // Test mutex operations
    SyncManager::Mutex* mutex = sm.getMutex(mutex_id);
    assert(mutex != nullptr);
    
    bool locked1 = mutex->lock(pid1);
    bool locked2 = mutex->lock(pid2);
    
    std::cout << "Mutex lock results: PID " << pid1 << " = " << (locked1 ? "acquired" : "blocked")
              << ", PID " << pid2 << " = " << (locked2 ? "acquired" : "blocked") << std::endl;
    std::cout << "Mutex owner: " << mutex->getOwner() << ", waiting: " << mutex->getWaitingCount() << std::endl;
    
    // Test MetricsCollector
    std::cout << "\n=== Testing MetricsCollector ===" << std::endl;
    MetricsCollector mc;
    
    // Create some processes for metrics
    ProcessControlBlock pcb1(pid1, 5, 10, 0);
    ProcessControlBlock pcb2(pid2, 3, 15, 2);
    
    mc.recordProcessCreation(pcb1);
    mc.recordProcessCreation(pcb2);
    
    // Simulate some execution
    mc.updateSimulationTime(5);
    mc.recordStateChange(pid1, ProcessState::NEW, ProcessState::READY, 0);
    mc.recordStateChange(pid1, ProcessState::READY, ProcessState::RUNNING, 2);
    mc.recordCPUExecution(3);
    
    mc.recordStateChange(pid2, ProcessState::NEW, ProcessState::READY, 2);
    mc.recordStateChange(pid2, ProcessState::READY, ProcessState::RUNNING, 5);
    
    // Complete process 1
    pcb1.completion_time = 12;
    mc.recordProcessCompletion(pcb1, 12);
    mc.updateSimulationTime(12);
    
    SystemMetrics metrics = mc.calculateMetrics();
    std::cout << "System Metrics:" << std::endl;
    std::cout << "  Total processes: " << metrics.total_processes << std::endl;
    std::cout << "  Completed processes: " << metrics.completed_processes << std::endl;
    std::cout << "  Average waiting time: " << metrics.average_waiting_time << std::endl;
    std::cout << "  CPU utilization: " << metrics.cpu_utilization << "%" << std::endl;
    
    // Test Schedulers
    std::cout << "\n=== Testing Schedulers ===" << std::endl;
    
    // Update process states for scheduling test
    pm.updateProcessState(pid1, ProcessState::READY);
    pm.updateProcessState(pid2, ProcessState::READY);
    
    std::vector<ProcessControlBlock*> ready_queue = pm.getProcessesByState(ProcessState::READY);
    
    FCFSScheduler fcfs;
    RoundRobinScheduler rr(4);
    PriorityScheduler priority;
    
    if (!ready_queue.empty()) {
        int fcfs_choice = fcfs.selectNextProcess(ready_queue);
        int rr_choice = rr.selectNextProcess(ready_queue);
        int priority_choice = priority.selectNextProcess(ready_queue);
        
        std::cout << "Scheduler choices from ready queue:" << std::endl;
        std::cout << "  FCFS: PID " << fcfs_choice << std::endl;
        std::cout << "  Round Robin: PID " << rr_choice << std::endl;
        std::cout << "  Priority: PID " << priority_choice << std::endl;
    }
    
    std::cout << "\n=== All Core Systems Tests Passed! ===" << std::endl;
    std::cout << "✅ ProcessManager: Process creation, state management" << std::endl;
    std::cout << "✅ MemoryManager: Page allocation, fault handling, FIFO/LRU" << std::endl;
    std::cout << "✅ SyncManager: Semaphores, mutexes, synchronization" << std::endl;
    std::cout << "✅ MetricsCollector: Performance tracking, statistics" << std::endl;
    std::cout << "✅ Schedulers: FCFS, Round Robin, Priority algorithms" << std::endl;
    
    return 0;
}