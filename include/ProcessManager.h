#pragma once

#include "ProcessState.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <mutex>

/**
 * @brief Manages process lifecycle and state transitions
 * 
 * The ProcessManager is responsible for creating, managing, and terminating
 * processes in the simulation. It maintains process control blocks and
 * ensures thread-safe operations on process data.
 */
class ProcessManager {
public:
    /**
     * @brief Constructor
     */
    ProcessManager();
    
    /**
     * @brief Destructor
     */
    ~ProcessManager();
    
    /**
     * @brief Create a new process
     * @param priority Process priority (1-10, lower = higher priority)
     * @param burst_time Total CPU time required
     * @param arrival_time Time when process arrives in system
     * @return Process ID of created process, or -1 if creation failed
     */
    int createProcess(int priority, int burst_time, int arrival_time);
    
    /**
     * @brief Terminate a process and clean up resources
     * @param pid Process ID to terminate
     * @return true if process was successfully terminated
     */
    bool terminateProcess(int pid);
    
    /**
     * @brief Update process state
     * @param pid Process ID
     * @param new_state New state to set
     * @return true if state was successfully updated
     */
    bool updateProcessState(int pid, ProcessState new_state);
    
    /**
     * @brief Get process control block by PID
     * @param pid Process ID
     * @return Pointer to ProcessControlBlock, or nullptr if not found
     */
    ProcessControlBlock* getProcess(int pid);
    
    /**
     * @brief Get all processes
     * @return Vector of pointers to all ProcessControlBlocks
     */
    std::vector<ProcessControlBlock*> getAllProcesses();
    
    /**
     * @brief Get processes in a specific state
     * @param state Process state to filter by
     * @return Vector of pointers to ProcessControlBlocks in the specified state
     */
    std::vector<ProcessControlBlock*> getProcessesByState(ProcessState state);
    
    /**
     * @brief Get total number of processes
     * @return Total process count
     */
    size_t getProcessCount() const;
    
    /**
     * @brief Get number of processes in a specific state
     * @param state Process state to count
     * @return Number of processes in the specified state
     */
    size_t getProcessCountByState(ProcessState state) const;
    
    /**
     * @brief Check if a process exists
     * @param pid Process ID to check
     * @return true if process exists
     */
    bool processExists(int pid) const;
    
    /**
     * @brief Reset all processes (clear all data)
     */
    void reset();
    
    /**
     * @brief Set process start time (when it first begins execution)
     * @param pid Process ID
     * @param start_time Time when process started
     */
    void setProcessStartTime(int pid, int start_time);
    
    /**
     * @brief Set process completion time
     * @param pid Process ID
     * @param completion_time Time when process completed
     */
    void setProcessCompletionTime(int pid, int completion_time);
    
    /**
     * @brief Decrease remaining time for a process
     * @param pid Process ID
     * @param time_units Time units to subtract
     * @return true if process still has remaining time, false if completed
     */
    bool decreaseRemainingTime(int pid, int time_units = 1);

private:
    std::unordered_map<int, std::unique_ptr<ProcessControlBlock>> processes_;
    int next_pid_;
    mutable std::mutex process_mutex_;
    
    /**
     * @brief Generate next available PID
     * @return Next process ID
     */
    int generateNextPID();
    
    /**
     * @brief Validate process parameters
     * @param priority Process priority
     * @param burst_time Burst time
     * @param arrival_time Arrival time
     * @return true if parameters are valid
     */
    bool validateProcessParameters(int priority, int burst_time, int arrival_time) const;
};