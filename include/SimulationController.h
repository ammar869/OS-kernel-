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
#include <condition_variable>

/**
 * @brief Controls the simulation execution and coordinates all kernel components
 * 
 * The SimulationController orchestrates the simulation by managing time steps,
 * coordinating between different kernel components, and maintaining simulation state.
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
     * @brief Start the simulation
     * @return true if started successfully
     */
    bool startSimulation();
    
    /**
     * @brief Pause the simulation
     */
    void pauseSimulation();
    
    /**
     * @brief Resume the simulation
     */
    void resumeSimulation();
    
    /**
     * @brief Stop the simulation
     */
    void stopSimulation();
    
    /**
     * @brief Reset the simulation to initial state
     */
    void resetSimulation();
    
    /**
     * @brief Execute a single time step
     * @return true if step executed successfully
     */
    bool executeStep();
    
    /**
     * @brief Set simulation configuration
     * @param config Simulation configuration
     */
    void setConfiguration(const SimulationConfig& config);
    
    /**
     * @brief Get current simulation configuration
     * @return Current configuration
     */
    const SimulationConfig& getConfiguration() const { return config_; }
    
    /**
     * @brief Get current simulation state
     * @return Current state
     */
    SimulationState getState() const { return state_; }
    
    /**
     * @brief Get current simulation time
     * @return Current time
     */
    int getCurrentTime() const { return current_time_; }
    
    /**
     * @brief Set simulation speed
     * @param steps_per_second Steps per second (1-10)
     */
    void setSimulationSpeed(int steps_per_second);
    
    /**
     * @brief Get simulation speed
     * @return Steps per second
     */
    int getSimulationSpeed() const { return simulation_speed_; }
    
    /**
     * @brief Enable/disable step mode
     * @param enabled Whether step mode is enabled
     */
    void setStepMode(bool enabled);
    
    /**
     * @brief Check if step mode is enabled
     * @return true if in step mode
     */
    bool isStepMode() const { return step_mode_; }
    
    /**
     * @brief Get process manager
     * @return Reference to process manager
     */
    ProcessManager& getProcessManager() { return *process_manager_; }
    
    /**
     * @brief Get memory manager
     * @return Reference to memory manager
     */
    MemoryManager& getMemoryManager() { return *memory_manager_; }
    
    /**
     * @brief Get synchronization manager
     * @return Reference to sync manager
     */
    SyncManager& getSyncManager() { return *sync_manager_; }
    
    /**
     * @brief Get metrics collector
     * @return Reference to metrics collector
     */
    MetricsCollector& getMetricsCollector() { return *metrics_collector_; }
    
    /**
     * @brief Get current system metrics
     * @return System metrics
     */
    SystemMetrics getSystemMetrics() const;
    
    /**
     * @brief Add a process to the simulation
     * @param priority Process priority
     * @param burst_time CPU burst time
     * @param arrival_time Arrival time
     * @param memory_pages Memory pages required
     * @return Process ID, or -1 if creation failed
     */
    int addProcess(int priority, int burst_time, int arrival_time, int memory_pages = 1);
    
    /**
     * @brief Set scheduling algorithm
     * @param algorithm Scheduling algorithm to use
     */
    void setSchedulingAlgorithm(SchedulingAlgorithm algorithm);
    
    /**
     * @brief Set time quantum for Round Robin
     * @param quantum Time quantum
     */
    void setTimeQuantum(int quantum);
    
    /**
     * @brief Set memory replacement algorithm
     * @param algorithm Memory replacement algorithm
     */
    void setMemoryReplacementAlgorithm(ReplacementAlgorithm algorithm);

private:
    // Core components
    std::unique_ptr<ProcessManager> process_manager_;
    std::unique_ptr<Scheduler> scheduler_;
    std::unique_ptr<MemoryManager> memory_manager_;
    std::unique_ptr<SyncManager> sync_manager_;
    std::unique_ptr<MetricsCollector> metrics_collector_;
    
    // Simulation state
    std::atomic<SimulationState> state_;
    std::atomic<int> current_time_;
    std::atomic<int> simulation_speed_;
    std::atomic<bool> step_mode_;
    
    // Configuration
    SimulationConfig config_;
    mutable std::mutex config_mutex_;
    
    // Threading
    std::unique_ptr<std::thread> simulation_thread_;
    std::mutex simulation_mutex_;
    std::condition_variable simulation_cv_;
    std::atomic<bool> should_stop_;
    
    // Current running process
    int current_running_process_;
    int current_process_remaining_quantum_;
    
    /**
     * @brief Main simulation loop
     */
    void simulationLoop();
    
    /**
     * @brief Process arrivals for current time
     */
    void processArrivals();
    
    /**
     * @brief Schedule next process
     */
    void scheduleProcess();
    
    /**
     * @brief Execute current process
     */
    void executeCurrentProcess();
    
    /**
     * @brief Handle process completion
     * @param pid Process ID that completed
     */
    void handleProcessCompletion(int pid);
    
    /**
     * @brief Update all metrics
     */
    void updateMetrics();
    
    /**
     * @brief Create scheduler instance
     * @param algorithm Scheduling algorithm
     * @return Unique pointer to scheduler
     */
    std::unique_ptr<Scheduler> createScheduler(SchedulingAlgorithm algorithm);
    
    /**
     * @brief Check if simulation should continue
     * @return true if simulation should continue
     */
    bool shouldContinueSimulation() const;
    
    /**
     * @brief Sleep for appropriate time based on simulation speed
     */
    void sleepForTimeStep();
};