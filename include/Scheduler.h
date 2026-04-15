#pragma once

#include "ProcessState.h"
#include <vector>
#include <memory>

/**
 * @brief Abstract base class for CPU scheduling algorithms
 * 
 * Defines the interface that all scheduling algorithms must implement.
 * Provides common functionality for process selection and preemption decisions.
 */
class Scheduler {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~Scheduler() = default;
    
    /**
     * @brief Select the next process to run from the ready queue
     * @param ready_queue Vector of processes in ready state
     * @return PID of selected process, or -1 if no process available
     */
    virtual int selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) = 0;
    
    /**
     * @brief Determine if current process should be preempted by a new process
     * @param current_pid PID of currently running process
     * @param new_pid PID of newly arrived process
     * @return true if preemption should occur
     */
    virtual bool shouldPreempt(int current_pid, int new_pid) = 0;
    
    /**
     * @brief Handle time quantum expiration (for Round Robin)
     */
    virtual void onTimeQuantumExpired() = 0;
    
    /**
     * @brief Reset scheduler state
     */
    virtual void reset() = 0;
    
    /**
     * @brief Get scheduler algorithm name
     * @return String name of the algorithm
     */
    virtual const char* getAlgorithmName() const = 0;
};

/**
 * @brief First-Come-First-Served (FCFS) scheduler implementation
 * 
 * Non-preemptive scheduling algorithm that selects processes based on
 * arrival time. The process that arrives first is served first.
 */
class FCFSScheduler : public Scheduler {
public:
    /**
     * @brief Constructor
     */
    FCFSScheduler();
    
    /**
     * @brief Destructor
     */
    ~FCFSScheduler() override = default;
    
    /**
     * @brief Select next process based on earliest arrival time
     * @param ready_queue Vector of processes in ready state
     * @return PID of process with earliest arrival time
     */
    int selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) override;
    
    /**
     * @brief FCFS is non-preemptive, so never preempts
     * @param current_pid Currently running process PID
     * @param new_pid Newly arrived process PID
     * @return Always false (no preemption)
     */
    bool shouldPreempt(int current_pid, int new_pid) override;
    
    /**
     * @brief No time quantum in FCFS
     */
    void onTimeQuantumExpired() override;
    
    /**
     * @brief Reset scheduler state
     */
    void reset() override;
    
    /**
     * @brief Get algorithm name
     * @return "FCFS"
     */
    const char* getAlgorithmName() const override;
};

/**
 * @brief Round Robin scheduler implementation
 * 
 * Preemptive scheduling algorithm that assigns a fixed time quantum
 * to each process and cycles through them in circular order.
 */
class RoundRobinScheduler : public Scheduler {
public:
    /**
     * @brief Constructor
     * @param time_quantum Time quantum for each process
     */
    explicit RoundRobinScheduler(int time_quantum = 4);
    
    /**
     * @brief Destructor
     */
    ~RoundRobinScheduler() override = default;
    
    /**
     * @brief Select next process in circular order
     * @param ready_queue Vector of processes in ready state
     * @return PID of next process in rotation
     */
    int selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) override;
    
    /**
     * @brief Round Robin doesn't preempt on arrival, only on quantum expiration
     * @param current_pid Currently running process PID
     * @param new_pid Newly arrived process PID
     * @return Always false (preemption only on quantum expiration)
     */
    bool shouldPreempt(int current_pid, int new_pid) override;
    
    /**
     * @brief Handle time quantum expiration
     */
    void onTimeQuantumExpired() override;
    
    /**
     * @brief Reset scheduler state
     */
    void reset() override;
    
    /**
     * @brief Get algorithm name
     * @return "Round Robin"
     */
    const char* getAlgorithmName() const override;
    
    /**
     * @brief Get time quantum
     * @return Current time quantum
     */
    int getTimeQuantum() const { return time_quantum_; }
    
    /**
     * @brief Set time quantum
     * @param quantum New time quantum value
     */
    void setTimeQuantum(int quantum);
    
    /**
     * @brief Get remaining time in current quantum
     * @return Remaining quantum time
     */
    int getRemainingQuantum() const { return current_quantum_remaining_; }
    
    /**
     * @brief Decrease remaining quantum time
     * @param time_units Time units to subtract
     * @return true if quantum still has time remaining
     */
    bool decreaseQuantum(int time_units = 1);

private:
    int time_quantum_;                ///< Time quantum for each process
    int current_quantum_remaining_;   ///< Remaining time in current quantum
    int last_scheduled_index_;        ///< Index of last scheduled process
};

/**
 * @brief Priority-based scheduler implementation
 * 
 * Preemptive scheduling algorithm that selects processes based on priority.
 * Lower priority numbers indicate higher priority.
 */
class PriorityScheduler : public Scheduler {
public:
    /**
     * @brief Constructor
     */
    PriorityScheduler();
    
    /**
     * @brief Destructor
     */
    ~PriorityScheduler() override = default;
    
    /**
     * @brief Select process with highest priority (lowest priority number)
     * @param ready_queue Vector of processes in ready state
     * @return PID of highest priority process
     */
    int selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) override;
    
    /**
     * @brief Check if new process has higher priority than current
     * @param current_pid Currently running process PID
     * @param new_pid Newly arrived process PID
     * @return true if new process has higher priority
     */
    bool shouldPreempt(int current_pid, int new_pid) override;
    
    /**
     * @brief No time quantum in priority scheduling
     */
    void onTimeQuantumExpired() override;
    
    /**
     * @brief Reset scheduler state
     */
    void reset() override;
    
    /**
     * @brief Get algorithm name
     * @return "Priority"
     */
    const char* getAlgorithmName() const override;

private:
    /**
     * @brief Get process priority by PID
     * @param pid Process ID
     * @return Process priority, or -1 if not found
     */
    int getProcessPriority(int pid) const;
};