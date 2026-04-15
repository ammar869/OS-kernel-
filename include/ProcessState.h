#pragma once

#include <vector>

/**
 * @brief Enumeration of possible process states in the simulation
 */
enum class ProcessState {
    NEW,        ///< Process has been created but not yet admitted to ready queue
    READY,      ///< Process is waiting to be assigned to processor
    RUNNING,    ///< Process is currently being executed
    WAITING,    ///< Process is waiting for some event to occur (I/O, synchronization)
    TERMINATED  ///< Process has finished execution
};

/**
 * @brief Convert ProcessState enum to string representation
 * @param state The process state to convert
 * @return String representation of the state
 */
const char* processStateToString(ProcessState state);

/**
 * @brief Process Control Block structure containing all process information
 */
struct ProcessControlBlock {
    int pid;                    ///< Process identifier
    ProcessState state;         ///< Current process state
    int priority;              ///< Process priority (1-10, lower = higher priority)
    int burst_time;            ///< Total CPU time required
    int remaining_time;        ///< Remaining CPU time
    int arrival_time;          ///< Time when process arrived in system
    int start_time;            ///< Time when process first started execution
    int completion_time;       ///< Time when process completed execution
    std::vector<int> allocated_pages;  ///< List of allocated memory pages
    
    // Constructor
    ProcessControlBlock(int id, int prio, int burst, int arrival)
        : pid(id), state(ProcessState::NEW), priority(prio), 
          burst_time(burst), remaining_time(burst), arrival_time(arrival),
          start_time(-1), completion_time(-1) {}
};

/**
 * @brief Process metrics for performance analysis
 */
struct ProcessMetrics {
    int waiting_time;      ///< Time spent in ready queue
    int turnaround_time;   ///< Total time from arrival to completion
    int response_time;     ///< Time from arrival to first execution
    int completion_time;   ///< Time when process completed
    
    ProcessMetrics() : waiting_time(0), turnaround_time(0), 
                      response_time(0), completion_time(0) {}
};