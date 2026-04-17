#include "SimulationController.h"
#include <algorithm>
#include <iostream>

SimulationController::SimulationController()
    : state_(SimulationState::STOPPED), current_time_(0), current_running_process_(-1),
      time_quantum_remaining_(0), should_stop_(false), simulation_speed_(1), step_mode_(false) {
    
    // Initialize core components
    process_manager_ = std::make_unique<ProcessManager>();
    memory_manager_ = std::make_unique<MemoryManager>();
    sync_manager_ = std::make_unique<SyncManager>();
    metrics_collector_ = std::make_unique<MetricsCollector>();
}

SimulationController::~SimulationController() {
    stop();
}

bool SimulationController::initialize(const SimulationConfig& config) {
    std::lock_guard<std::mutex> lock(simulation_mutex_);
    
    if (state_ != SimulationState::STOPPED) {
        return false;
    }
    
    config_ = config;
    
    // Initialize memory manager with configuration
    memory_manager_ = std::make_unique<MemoryManager>(config.memory_config);
    
    // Create scheduler
    current_scheduler_ = createScheduler(config.scheduling_algo, config.time_quantum);
    
    // Set simulation parameters
    simulation_speed_ = config.simulation_speed;
    step_mode_ = config.step_mode;
    
    // Add processes from configuration
    for (const auto& process_config : config.processes) {
        addProcess(process_config);
    }
    
    return true;
}

bool SimulationController::start() {
    std::lock_guard<std::mutex> lock(simulation_mutex_);
    
    if (state_ != SimulationState::STOPPED && state_ != SimulationState::PAUSED) {
        return false;
    }
    
    if (step_mode_) {
        state_ = SimulationState::STEP_MODE;
        return true;
    }
    
    state_ = SimulationState::RUNNING;
    should_stop_ = false;
    
    // Start simulation thread
    simulation_thread_ = std::make_unique<std::thread>(&SimulationController::simulationLoop, this);
    
    return true;
}

void SimulationController::pause() {
    if (state_ == SimulationState::RUNNING) {
        state_ = SimulationState::PAUSED;
    }
}

void SimulationController::resume() {
    if (state_ == SimulationState::PAUSED) {
        state_ = SimulationState::RUNNING;
    }
}

void SimulationController::stop() {
    should_stop_ = true;
    state_ = SimulationState::STOPPED;
    
    if (simulation_thread_ && simulation_thread_->joinable()) {
        simulation_thread_->join();
        simulation_thread_.reset();
    }
}

void SimulationController::reset() {
    stop();
    
    std::lock_guard<std::mutex> lock(simulation_mutex_);
    
    // Reset all components
    process_manager_->reset();
    memory_manager_->reset();
    sync_manager_->reset();
    metrics_collector_->reset();
    
    if (current_scheduler_) {
        current_scheduler_->reset();
    }
    
    // Reset simulation state
    current_time_ = 0;
    current_running_process_ = -1;
    time_quantum_remaining_ = 0;
    state_ = SimulationState::STOPPED;
}

bool SimulationController::executeStep() {
    if (state_ != SimulationState::STEP_MODE) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(simulation_mutex_);
    return executeTimeStep();
}

void SimulationController::setSimulationSpeed(int steps_per_second) {
    if (steps_per_second >= 1 && steps_per_second <= 10) {
        simulation_speed_ = steps_per_second;
    }
}

void SimulationController::setStepMode(bool enabled) {
    step_mode_ = enabled;
    
    if (enabled && state_ == SimulationState::RUNNING) {
        pause();
        state_ = SimulationState::STEP_MODE;
    }
}

SimulationController::SimulationState SimulationController::getState() const {
    return state_;
}

int SimulationController::getCurrentTime() const {
    return current_time_;
}

const SimulationConfig& SimulationController::getConfiguration() const {
    return config_;
}

void SimulationController::setSchedulingAlgorithm(SchedulingAlgorithm algorithm, int time_quantum) {
    std::lock_guard<std::mutex> lock(simulation_mutex_);
    
    config_.scheduling_algo = algorithm;
    config_.time_quantum = time_quantum;
    current_scheduler_ = createScheduler(algorithm, time_quantum);
    
    // Reset current running process to force rescheduling
    current_running_process_ = -1;
    time_quantum_remaining_ = 0;
}

int SimulationController::addProcess(const ProcessConfig& config) {
    int pid = process_manager_->createProcess(config.priority, config.burst_time, config.arrival_time);
    
    if (pid > 0) {
        // Record process creation in metrics
        ProcessControlBlock* pcb = process_manager_->getProcess(pid);
        if (pcb) {
            metrics_collector_->recordProcessCreation(*pcb);
            
            // Allocate memory if needed
            if (config.memory_requirement > 0) {
                memory_manager_->allocatePages(pid, config.memory_requirement);
            }
        }
    }
    
    return pid;
}

std::vector<ProcessControlBlock*> SimulationController::getAllProcesses() {
    return process_manager_->getAllProcesses();
}

SystemMetrics SimulationController::getSystemMetrics() {
    // Update memory statistics in metrics collector
    metrics_collector_->recordMemoryStats(
        memory_manager_->getPageFaultCount(),
        memory_manager_->getPageReplacementCount(),
        memory_manager_->getMemoryUtilization()
    );
    
    return metrics_collector_->calculateMetrics();
}

void SimulationController::simulationLoop() {
    while (!should_stop_ && state_ != SimulationState::STOPPED) {
        if (state_ == SimulationState::RUNNING) {
            {
                std::lock_guard<std::mutex> lock(simulation_mutex_);
                if (!executeTimeStep()) {
                    // Simulation completed
                    state_ = SimulationState::STOPPED;
                    break;
                }
            }
            
            // Sleep based on simulation speed
            std::this_thread::sleep_for(getSleepDuration());
        } else if (state_ == SimulationState::PAUSED) {
            // Wait while paused
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

bool SimulationController::executeTimeStep() {
    // Update simulation time
    current_time_++;
    metrics_collector_->updateSimulationTime(current_time_);
    
    // Handle process arrivals
    handleProcessArrivals();
    
    // Handle CPU scheduling
    handleCPUScheduling();
    
    // Handle process execution
    handleProcessExecution();
    
    // Handle memory management
    handleMemoryManagement();
    
    // Handle synchronization
    handleSynchronization();
    
    // Update metrics
    updateMetrics();
    
    // Check if simulation should continue
    return !allProcessesCompleted();
}

void SimulationController::handleProcessArrivals() {
    // Move processes that have arrived to READY state
    auto all_processes = process_manager_->getAllProcesses();
    
    for (auto* pcb : all_processes) {
        if (pcb->state == ProcessState::NEW && pcb->arrival_time <= current_time_) {
            ProcessState old_state = pcb->state;
            process_manager_->updateProcessState(pcb->pid, ProcessState::READY);
            metrics_collector_->recordStateChange(pcb->pid, old_state, ProcessState::READY, current_time_);
        }
    }
}

void SimulationController::handleCPUScheduling() {
    if (!current_scheduler_) {
        return;
    }
    
    // Get ready processes
    auto ready_processes = process_manager_->getProcessesByState(ProcessState::READY);
    
    // Check if we need to schedule a new process
    bool need_scheduling = false;
    
    if (current_running_process_ == -1) {
        // No process currently running
        need_scheduling = true;
    } else {
        // Check if current process is still running
        ProcessControlBlock* current_pcb = process_manager_->getProcess(current_running_process_);
        if (!current_pcb || current_pcb->state != ProcessState::RUNNING) {
            current_running_process_ = -1;
            need_scheduling = true;
        } else if (config_.scheduling_algo == SchedulingAlgorithm::ROUND_ROBIN && time_quantum_remaining_ <= 0) {
            // Time quantum expired for Round Robin
            ProcessState old_state = current_pcb->state;
            process_manager_->updateProcessState(current_running_process_, ProcessState::READY);
            metrics_collector_->recordStateChange(current_running_process_, old_state, ProcessState::READY, current_time_);
            current_running_process_ = -1;
            need_scheduling = true;
        }
    }
    
    // Schedule new process if needed
    if (need_scheduling && !ready_processes.empty()) {
        int selected_pid = current_scheduler_->selectNextProcess(ready_processes);
        
        if (selected_pid > 0) {
            ProcessControlBlock* selected_pcb = process_manager_->getProcess(selected_pid);
            if (selected_pcb) {
                ProcessState old_state = selected_pcb->state;
                process_manager_->updateProcessState(selected_pid, ProcessState::RUNNING);
                metrics_collector_->recordStateChange(selected_pid, old_state, ProcessState::RUNNING, current_time_);
                
                current_running_process_ = selected_pid;
                
                // Set time quantum for Round Robin
                if (config_.scheduling_algo == SchedulingAlgorithm::ROUND_ROBIN) {
                    time_quantum_remaining_ = config_.time_quantum;
                }
            }
        }
    }
}

void SimulationController::handleProcessExecution() {
    if (current_running_process_ == -1) {
        return;
    }
    
    ProcessControlBlock* running_pcb = process_manager_->getProcess(current_running_process_);
    if (!running_pcb || running_pcb->state != ProcessState::RUNNING) {
        current_running_process_ = -1;
        return;
    }
    
    // Execute process for one time unit
    bool still_has_time = process_manager_->decreaseRemainingTime(current_running_process_, 1);
    metrics_collector_->recordCPUExecution(1);
    
    // Decrease time quantum for Round Robin
    if (config_.scheduling_algo == SchedulingAlgorithm::ROUND_ROBIN) {
        time_quantum_remaining_--;
    }
    
    // Check if process completed
    if (!still_has_time) {
        ProcessState old_state = running_pcb->state;
        process_manager_->updateProcessState(current_running_process_, ProcessState::TERMINATED);
        process_manager_->setProcessCompletionTime(current_running_process_, current_time_);
        
        // Record completion in metrics
        ProcessControlBlock* completed_pcb = process_manager_->getProcess(current_running_process_);
        if (completed_pcb) {
            metrics_collector_->recordProcessCompletion(*completed_pcb, current_time_);
        }
        
        // Clean up resources
        memory_manager_->deallocatePages(current_running_process_);
        sync_manager_->cleanupProcess(current_running_process_);
        
        current_running_process_ = -1;
        time_quantum_remaining_ = 0;
    }
}

void SimulationController::handleMemoryManagement() {
    // Update memory access times for LRU
    if (current_running_process_ != -1) {
        // Simulate memory access by the running process
        memory_manager_->accessPage(current_running_process_, 0, current_time_);
    }
}

void SimulationController::handleSynchronization() {
    // Check for deadlocks
    if (sync_manager_->detectDeadlock()) {
        std::cout << "Warning: Potential deadlock detected at time " << current_time_ << std::endl;
    }
}

void SimulationController::updateMetrics() {
    // Update metrics collector with current time
    metrics_collector_->updateSimulationTime(current_time_);
    
    // Update memory statistics
    metrics_collector_->recordMemoryStats(
        memory_manager_->getPageFaultCount(),
        memory_manager_->getPageReplacementCount(),
        memory_manager_->getMemoryUtilization()
    );
}

std::unique_ptr<Scheduler> SimulationController::createScheduler(SchedulingAlgorithm algorithm, int time_quantum) {
    switch (algorithm) {
        case SchedulingAlgorithm::FCFS:
            return std::make_unique<FCFSScheduler>();
        case SchedulingAlgorithm::ROUND_ROBIN:
            return std::make_unique<RoundRobinScheduler>(time_quantum);
        case SchedulingAlgorithm::PRIORITY:
            return std::make_unique<PriorityScheduler>();
        default:
            return std::make_unique<FCFSScheduler>();
    }
}

bool SimulationController::allProcessesCompleted() const {
    auto all_processes = process_manager_->getAllProcesses();
    
    for (const auto* pcb : all_processes) {
        if (pcb->state != ProcessState::TERMINATED) {
            return false;
        }
    }
    
    return !all_processes.empty(); // Return true only if there were processes and all are completed
}

std::chrono::milliseconds SimulationController::getSleepDuration() const {
    // Convert steps per second to milliseconds per step
    int speed = simulation_speed_.load();
    return std::chrono::milliseconds(1000 / speed);
}