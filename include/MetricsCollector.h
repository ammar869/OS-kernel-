#pragma once

#include "ProcessState.h"
#include "SimulationConfig.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <string>

/**
 * @brief Collects and calculates performance metrics for the simulation
 * 
 * The MetricsCollector tracks process execution statistics and calculates
 * various performance metrics including waiting time, turnaround time,
 * CPU utilization, and throughput.
 */
class MetricsCollector {
public:
    /**
     * @brief Constructor
     */
    MetricsCollector();
    
    /**
     * @brief Destructor
     */
    ~MetricsCollector() = default;
    
    /**
     * @brief Record process creation
     * @param process Process control block
     */
    void recordProcessCreation(const ProcessControlBlock& process);
    
    /**
     * @brief Record process completion
     * @param process Process control block
     * @param completion_time Time when process completed
     */
    void recordProcessCompletion(const ProcessControlBlock& process, int completion_time);
    
    /**
     * @brief Record process state change
     * @param pid Process ID
     * @param old_state Previous state
     * @param new_state New state
     * @param current_time Current simulation time
     */
    void recordStateChange(int pid, ProcessState old_state, ProcessState new_state, int current_time);
    
    /**
     * @brief Update simulation time
     * @param current_time Current simulation time
     */
    void updateSimulationTime(int current_time);
    
    /**
     * @brief Record CPU execution time
     * @param time_units Time units of CPU execution
     */
    void recordCPUExecution(int time_units);
    
    /**
     * @brief Record memory statistics
     * @param page_faults Number of page faults
     * @param page_replacements Number of page replacements
     * @param memory_utilization Memory utilization percentage
     */
    void recordMemoryStats(int page_faults, int page_replacements, double memory_utilization);
    
    /**
     * @brief Calculate and return current system metrics
     * @return SystemMetrics structure with all calculated metrics
     */
    SystemMetrics calculateMetrics() const;
    
    /**
     * @brief Get metrics for a specific process
     * @param pid Process ID
     * @return ProcessMetrics for the specified process, or default if not found
     */
    ProcessMetrics getProcessMetrics(int pid) const;
    
    /**
     * @brief Get all process metrics
     * @return Map of PID to ProcessMetrics
     */
    std::unordered_map<int, ProcessMetrics> getAllProcessMetrics() const;
    
    /**
     * @brief Reset all metrics
     */
    void reset();
    
    /**
     * @brief Generate comprehensive performance report
     * @return String containing detailed performance report
     */
    std::string generateReport() const;
    
    /**
     * @brief Get average waiting time
     * @return Average waiting time across all completed processes
     */
    double getAverageWaitingTime() const;
    
    /**
     * @brief Get average turnaround time
     * @return Average turnaround time across all completed processes
     */
    double getAverageTurnaroundTime() const;
    
    /**
     * @brief Get average response time
     * @return Average response time across all completed processes
     */
    double getAverageResponseTime() const;
    
    /**
     * @brief Get CPU utilization percentage
     * @return CPU utilization as percentage (0-100)
     */
    double getCPUUtilization() const;
    
    /**
     * @brief Get throughput
     * @return Number of processes completed per time unit
     */
    double getThroughput() const;
    
    /**
     * @brief Get total number of processes
     * @return Total process count
     */
    int getTotalProcesses() const;
    
    /**
     * @brief Get number of completed processes
     * @return Completed process count
     */
    int getCompletedProcesses() const;
    
    /**
     * @brief Get current simulation time
     * @return Current time
     */
    int getCurrentTime() const;

private:
    struct ProcessTrackingInfo {
        ProcessControlBlock pcb;
        int first_run_time;     ///< Time when process first started running
        int total_wait_time;    ///< Total time spent waiting
        int last_state_change_time; ///< Time of last state change
        bool has_started;       ///< Whether process has started execution
        
        ProcessTrackingInfo(const ProcessControlBlock& process)
            : pcb(process), first_run_time(-1), total_wait_time(0),
              last_state_change_time(process.arrival_time), has_started(false) {}
    };
    
    mutable std::mutex metrics_mutex_;
    
    // Process tracking
    std::unordered_map<int, ProcessTrackingInfo> process_info_;
    std::unordered_map<int, ProcessMetrics> completed_processes_;
    
    // System metrics
    int current_simulation_time_;
    int total_cpu_execution_time_;
    int total_processes_;
    int completed_processes_count_;
    
    // Memory metrics
    int total_page_faults_;
    int total_page_replacements_;
    double current_memory_utilization_;
    
    /**
     * @brief Calculate waiting time for a process
     * @param info Process tracking information
     * @return Waiting time
     */
    int calculateWaitingTime(const ProcessTrackingInfo& info) const;
    
    /**
     * @brief Calculate turnaround time for a process
     * @param info Process tracking information
     * @return Turnaround time
     */
    int calculateTurnaroundTime(const ProcessTrackingInfo& info) const;
    
    /**
     * @brief Calculate response time for a process
     * @param info Process tracking information
     * @return Response time
     */
    int calculateResponseTime(const ProcessTrackingInfo& info) const;
    
    /**
     * @brief Update process waiting time
     * @param pid Process ID
     * @param current_time Current simulation time
     */
    void updateWaitingTime(int pid, int current_time);
};