#pragma once

#include "ProcessManager.h"
#include "Scheduler.h"
#include "MemoryManager.h"
#include "SyncManager.h"
#include "MetricsCollector.h"
#include "SimulationConfig.h"
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

/**
 * @brief Controls the simulation execution and timing
 * 
 * The SimulationController orchestrates all kernel components and manages
 * the discrete time step execution of the simulation.
 */
class SimulationController {
public:
    enum class SimulationState {
        STOPPED,
        RUNNING,
        PAUSED,
        STEP_MODE
    };

    /**
     * @brief Constructor
     */
    SimulationController();
    
    /**
     * @brief Destructor
     */
    ~SimulationController();
    
    /**
     * @brief Initialize simulation with configuration
     * @param config Simulation configuration
     * @return true if initialization successful
     */
    bool initialize(const SimulationConfig& config);
    
    /**
     * @brief Start the simulation
     * @return true if started successfully
     */
    bool start();
    
    /**
     * @brief Pause the simulation
     */
    void pause();
    
    /**
     * @brief Resume the simulation
     */
    void resume();
    
    /**
     * @brief Stop the simulation
     */
    void stop();
    
    /**
     * @brief Reset the simulation to initial state
     */
    void reset();
    
    /**
     * @brief Execute a single time step (for step mode)
     * @return true if step executed successfully
     */
    bool executeStep();
    
    /**
     * @brief Set simulation speed
     * @param steps_per_second Steps per second (1-10)
     */
    void setSimulationSpeed(int steps_per_second);
    
    /**
     * @brief Enable/disable step mode
     * @param enabled Whether step mode is enabled
     */
    void setStepMode(bool enabled);
    
    /**
     * @brief Get current simulation state
     * @return Current state
     */
    SimulationState getState() const;
    
    /**
     * @brief Get current simulation time
     * @return Current time
     */
    int getCurrentTime() const;
    
    /**
     * @brief Get simulation configuration
     * @return Current configuration
     */
    const SimulationConfig& getConfiguration() const;
    
    /**
     * @brief Get process manager
     * @return Pointer to process manager
     */
    ProcessManager* getProcessManager() { return process_manager_.get(); }
    
    /**
     * @brief Get memory manager
     * @return Pointer to memory manager
     */
    MemoryManager* getMemoryManager() { return memory_manager_.get(); }
    
    /**
     * @brief Get sync manager
     * @return Pointer to sync manager
     */
    SyncManager* getSyncManager() { return sync_manager_.get(); }
    
    /**
     * @brief Get metrics collector
     * @return Pointer to metrics collector
     */
    MetricsCollector* getMetricsCollector() { return metrics_collector_.get(); }
    
    /**
     * @brief Get current scheduler
     * @return Pointer to current scheduler
     */
    Scheduler* getCurrentScheduler() { return current_scheduler_.get(); }
    
    /**
     * @brief Change scheduling algorithm
     * @param algorithm New scheduling algorithm
     * @param time_quantum Time quantum for Round Robin (ignored for others)
     */
    void setSchedulingAlgorithm(SchedulingAlgorithm algorithm, int time_quantum = 4);
    
    /**
     * @brief Add a process to the simulation
     * @param config Process configuration
     * @return Process ID, or -1 if failed
     */
    int addProcess(const ProcessConfig& config);
    
    /**
     * @brief Get all processes
     * @return Vector of all process control blocks
     */
    std::vector<ProcessControlBlock*> getAllProcesses();
    
    /**
     * @brief Get system metrics
     * @return Current system metrics
     */
    SystemMetrics getSystemMetrics();

private:
    // Core components
    std::unique_ptr<ProcessManager> process_manager_;
    std::unique_ptr<MemoryManager> memory_manager_;
    std::unique_ptr<SyncManager> sync_manager_;
    std::unique_ptr<MetricsCollector> metrics_collector_;
    std::unique_ptr<Scheduler> current_scheduler_;
    
    // Simulation state
    std::atomic<SimulationState> state_;
    SimulationConfig config_;
    int current_time_;
    int current_running_process_;
    int time_quantum_remaining_;
    
    // Threading
    std::unique_ptr<std::thread> simulation_thread_;
    std::mutex simulation_mutex_;
    std::atomic<bool> should_stop_;
    std::atomic<int> simulation_speed_;
    std::atomic<bool> step_mode_;
    
    /**
     * @brief Main simulation loop
     */
    void simulationLoop();
    
    /**
     * @brief Execute one time step of simulation
     * @return true if simulation should continue
     */
    bool executeTimeStep();
    
    /**
     * @brief Handle process arrivals at current time
     */
    void handleProcessArrivals();
    
    /**
     * @brief Handle CPU scheduling
     */
    void handleCPUScheduling();
    
    /**
     * @brief Handle process execution
     */
    void handleProcessExecution();
    
    /**
     * @brief Handle memory management
     */
    void handleMemoryManagement();
    
    /**
     * @brief Handle synchronization
     */
    void handleSynchronization();
    
    /**
     * @brief Update metrics
     */
    void updateMetrics();
    
    /**
     * @brief Create scheduler instance
     * @param algorithm Scheduling algorithm
     * @param time_quantum Time quantum for Round Robin
     * @return Unique pointer to scheduler
     */
    std::unique_ptr<Scheduler> createScheduler(SchedulingAlgorithm algorithm, int time_quantum);
    
    /**
     * @brief Check if all processes are completed
     * @return true if all processes are terminated
     */
    bool allProcessesCompleted() const;
    
    /**
     * @brief Get sleep duration based on simulation speed
     * @return Sleep duration in milliseconds
     */
    std::chrono::milliseconds getSleepDuration() const;
};