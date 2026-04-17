/**
 * Integration tests for Mini OS Kernel Simulator
 * Tests end-to-end simulation scenarios with all kernel components.
 */

#include "ProcessManager.h"
#include "Scheduler.h"
#include "MemoryManager.h"
#include "SyncManager.h"
#include "MetricsCollector.h"
#include "SimulationController.h"
#include "ConfigurationManager.h"
#include "InputValidator.h"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>

// ---- Helpers ----
static int pass_count = 0;
static int fail_count = 0;

#define TEST(name, expr) do { \
    if (expr) { std::cout << "  [PASS] " << name << "\n"; pass_count++; } \
    else       { std::cout << "  [FAIL] " << name << "\n"; fail_count++; } \
} while(0)

// ============================================================
// 1. Process Management Integration
// ============================================================
void test_process_management() {
    std::cout << "\n=== Process Management ===\n";
    ProcessManager pm;

    int pid1 = pm.createProcess(5, 10, 0);
    int pid2 = pm.createProcess(3, 8, 2);
    int pid3 = pm.createProcess(7, 6, 4);

    TEST("Create 3 unique PIDs", pid1 > 0 && pid2 > 0 && pid3 > 0 && pid1 != pid2 && pid2 != pid3);
    TEST("Process count = 3", pm.getProcessCount() == 3);

    pm.updateProcessState(pid1, ProcessState::READY);
    pm.updateProcessState(pid2, ProcessState::READY);
    TEST("Ready count = 2", pm.getProcessCountByState(ProcessState::READY) == 2);

    pm.updateProcessState(pid1, ProcessState::RUNNING);
    TEST("Running count = 1", pm.getProcessCountByState(ProcessState::RUNNING) == 1);

    pm.setProcessStartTime(pid1, 0);
    pm.setProcessCompletionTime(pid1, 10);
    bool still_running = pm.decreaseRemainingTime(pid1, 10);
    TEST("Remaining time reaches 0", !still_running);

    pm.terminateProcess(pid1);
    TEST("Process count after termination = 2", pm.getProcessCount() == 2);

    // Invalid params
    int bad = pm.createProcess(15, 10, 0); // priority out of range
    TEST("Invalid priority rejected", bad == -1);
}

// ============================================================
// 2. Scheduling Algorithms Integration
// ============================================================
void test_scheduling() {
    std::cout << "\n=== Scheduling Algorithms ===\n";
    ProcessManager pm;

    int p1 = pm.createProcess(5, 10, 0);
    int p2 = pm.createProcess(3, 8, 2);
    int p3 = pm.createProcess(1, 6, 1);

    pm.updateProcessState(p1, ProcessState::READY);
    pm.updateProcessState(p2, ProcessState::READY);
    pm.updateProcessState(p3, ProcessState::READY);

    auto ready = pm.getProcessesByState(ProcessState::READY);

    // FCFS: earliest arrival first
    FCFSScheduler fcfs;
    int fcfs_choice = fcfs.selectNextProcess(ready);
    TEST("FCFS selects earliest arrival (p1, arrival=0)", fcfs_choice == p1);

    // Priority: lowest priority number first
    PriorityScheduler prio;
    int prio_choice = prio.selectNextProcess(ready);
    TEST("Priority selects highest priority (p3, priority=1)", prio_choice == p3);

    // Round Robin: cycles through
    RoundRobinScheduler rr(4);
    TEST("RR time quantum = 4", rr.getTimeQuantum() == 4);
    int rr1 = rr.selectNextProcess(ready);
    int rr2 = rr.selectNextProcess(ready);
    TEST("RR selects different processes in sequence", rr1 != rr2);

    // Quantum decrement
    rr.decreaseQuantum(4);
    TEST("RR quantum exhausted", rr.getRemainingQuantum() == 0);
}

// ============================================================
// 3. Memory Management Integration
// ============================================================
void test_memory_management() {
    std::cout << "\n=== Memory Management ===\n";
    MemoryConfiguration config(256, 32); // 8 frames
    MemoryManager mm(config);

    TEST("Initial free frames = 8", mm.getFreeFrameCount() == 8);

    bool ok1 = mm.allocatePages(1, 3);
    bool ok2 = mm.allocatePages(2, 2);
    TEST("Allocate 3 pages for P1", ok1);
    TEST("Allocate 2 pages for P2", ok2);
    TEST("Free frames = 3 after allocation", mm.getFreeFrameCount() == 3);
    TEST("Memory utilization > 0", mm.getMemoryUtilization() > 0.0);

    // Page fault
    int frame = mm.handlePageFault(1, 10);
    TEST("Page fault handled, frame assigned", frame >= 0);
    TEST("Page fault count = 1", mm.getPageFaultCount() == 1);

    // LRU replacement
    mm.allocatePages(3, 3); // Fill remaining frames
    int replaced_frame = mm.handlePageFault(3, 99);
    TEST("LRU replacement occurs when full", replaced_frame >= 0);
    TEST("Page replacement count >= 1", mm.getPageReplacementCount() >= 1);

    // Deallocation
    mm.deallocatePages(1);
    TEST("Free frames increase after deallocation", mm.getFreeFrameCount() > 0);

    // FIFO replacement
    MemoryManager mm_fifo(MemoryConfiguration(128, 32, ReplacementAlgorithm::FIFO));
    mm_fifo.allocatePages(1, 4); // Fill all 4 frames
    int fifo_frame = mm_fifo.handlePageFault(2, 0);
    TEST("FIFO replacement works", fifo_frame >= 0);
}

// ============================================================
// 4. Synchronization Integration
// ============================================================
void test_synchronization() {
    std::cout << "\n=== Synchronization ===\n";
    SyncManager sm;

    int sem_id = sm.createSemaphore(2);
    TEST("Semaphore created", sem_id > 0);

    auto* sem = sm.getSemaphore(sem_id);
    TEST("Semaphore retrieved", sem != nullptr);

    bool a1 = sem->wait(1);
    bool a2 = sem->wait(2);
    bool a3 = sem->wait(3); // Should block
    TEST("First two waits succeed", a1 && a2);
    TEST("Third wait blocks (count=0)", !a3);
    TEST("Semaphore count = 0", sem->getCount() == 0);
    TEST("One process waiting", sem->getWaitingCount() == 1);

    int woken = sem->signal();
    TEST("Signal wakes waiting process", woken == 3);

    // Mutex
    int mtx_id = sm.createMutex();
    auto* mtx = sm.getMutex(mtx_id);
    TEST("Mutex created", mtx != nullptr);

    bool l1 = mtx->lock(10);
    bool l2 = mtx->lock(20); // Should block
    TEST("First lock succeeds", l1);
    TEST("Second lock blocks", !l2);
    TEST("Mutex owner = 10", mtx->getOwner() == 10);

    int next_owner = mtx->unlock(10);
    TEST("Unlock transfers to waiting process", next_owner == 20);
    TEST("New owner = 20", mtx->getOwner() == 20);

    // Cleanup
    sm.cleanupProcess(20);
    TEST("Cleanup releases mutex", mtx->getOwner() == -1);
}

// ============================================================
// 5. Metrics Integration
// ============================================================
void test_metrics() {
    std::cout << "\n=== Metrics Collector ===\n";
    MetricsCollector mc;

    ProcessControlBlock p1(1, 5, 10, 0);
    ProcessControlBlock p2(2, 3, 8, 2);
    mc.recordProcessCreation(p1);
    mc.recordProcessCreation(p2);
    TEST("Total processes = 2", mc.getTotalProcesses() == 2);

    mc.recordStateChange(1, ProcessState::NEW, ProcessState::READY, 0);
    mc.recordStateChange(1, ProcessState::READY, ProcessState::RUNNING, 2);
    mc.recordCPUExecution(8);
    mc.updateSimulationTime(10);

    p1.completion_time = 10;
    mc.recordProcessCompletion(p1, 10);
    TEST("Completed processes = 1", mc.getCompletedProcesses() == 1);

    SystemMetrics m = mc.calculateMetrics();
    TEST("CPU utilization > 0", m.cpu_utilization > 0.0);
    TEST("Average waiting time >= 0", m.average_waiting_time >= 0.0);
    TEST("Turnaround time > 0", m.average_turnaround_time > 0.0);

    mc.reset();
    TEST("Reset clears processes", mc.getTotalProcesses() == 0);
}

// ============================================================
// 6. Configuration Manager Integration
// ============================================================
void test_configuration() {
    std::cout << "\n=== Configuration Manager ===\n";
    ConfigurationManager cm;

    SimulationConfig config = ConfigurationManager::createDefaultConfiguration();
    TEST("Default config has processes", !config.processes.empty());
    TEST("Default scheduling is FCFS", config.scheduling_algo == SchedulingAlgorithm::FCFS);

    // Save and reload
    bool saved = cm.saveConfiguration(config, "test_config.json");
    TEST("Configuration saved", saved);

    SimulationConfig loaded;
    bool loaded_ok = cm.loadConfiguration("test_config.json", loaded);
    TEST("Configuration loaded", loaded_ok);
    TEST("Scheduling algo preserved", loaded.scheduling_algo == config.scheduling_algo);
    TEST("Time quantum preserved", loaded.time_quantum == config.time_quantum);
    TEST("Process count preserved", loaded.processes.size() == config.processes.size());

    // Cleanup
    std::remove("test_config.json");
}

// ============================================================
// 7. Performance Test (100ms time step requirement)
// ============================================================
void test_performance() {
    std::cout << "\n=== Performance Test ===\n";
    ProcessManager pm;
    MemoryManager mm(MemoryConfiguration(1024, 32));
    SyncManager sm;
    MetricsCollector mc;
    FCFSScheduler scheduler;

    // Create 100 processes
    for (int i = 0; i < 100; ++i) {
        int pid = pm.createProcess((i % 10) + 1, (i % 20) + 1, i);
        ProcessControlBlock* pcb = pm.getProcess(pid);
        if (pcb) mc.recordProcessCreation(*pcb);
    }
    TEST("100 processes created", pm.getProcessCount() == 100);

    // Measure one time step
    auto start = std::chrono::high_resolution_clock::now();

    pm.updateProcessState(1, ProcessState::READY);
    auto ready = pm.getProcessesByState(ProcessState::READY);
    if (!ready.empty()) {
        scheduler.selectNextProcess(ready);
    }
    mm.allocatePages(1, 2);
    mc.updateSimulationTime(1);

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    TEST("Time step completes within 100ms", ms < 100);
    std::cout << "  Time step duration: " << ms << "ms\n";
}

// ============================================================
// 8. Input Validation
// ============================================================
void test_validation() {
    std::cout << "\n=== Input Validation ===\n";
    TEST("Valid params accepted", InputValidator::validateProcessParameters(5, 10, 0).is_valid);
    TEST("Priority 0 rejected",   !InputValidator::validateProcessParameters(0, 10, 0).is_valid);
    TEST("Priority 11 rejected",  !InputValidator::validateProcessParameters(11, 10, 0).is_valid);
    TEST("Burst 0 rejected",      !InputValidator::validateProcessParameters(5, 0, 0).is_valid);
    TEST("Negative arrival rejected", !InputValidator::validateProcessParameters(5, 10, -1).is_valid);
    TEST("Valid memory config",   InputValidator::validateMemoryConfiguration(256, 32).is_valid);
    TEST("Non-power-of-2 page rejected", !InputValidator::validateMemoryConfiguration(256, 30).is_valid);
    TEST("Valid time quantum",    InputValidator::validateTimeQuantum(4).is_valid);
    TEST("Zero quantum rejected", !InputValidator::validateTimeQuantum(0).is_valid);
    TEST("Valid speed",           InputValidator::validateSimulationSpeed(5).is_valid);
    TEST("Speed 11 rejected",     !InputValidator::validateSimulationSpeed(11).is_valid);
    TEST("Process count 100 ok",  InputValidator::validateProcessCount(100).is_valid);
    TEST("Process count 101 rejected", !InputValidator::validateProcessCount(101).is_valid);
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "========================================\n";
    std::cout << " Mini OS Kernel Simulator - Integration Tests\n";
    std::cout << "========================================\n";

    test_process_management();
    test_scheduling();
    test_memory_management();
    test_synchronization();
    test_metrics();
    test_configuration();
    test_performance();
    test_validation();

    std::cout << "\n========================================\n";
    std::cout << " Results: " << pass_count << " passed, " << fail_count << " failed\n";
    std::cout << "========================================\n";

    return fail_count == 0 ? 0 : 1;
}