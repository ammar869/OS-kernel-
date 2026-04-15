#include "SimulationController.h"
#include <chrono>
#include <algorithm>

SimulationController::SimulationController()
    : state_(SimulationState::STOPPED), current_time_(0), simulation_speed_(1),
      step_mode_(false), should_stop_(false), current_running_process_(-1),
      current_process_remaining_quantum_(0) {
    
    // Initialize core components
    process_manager_ = std::make_unique<ProcessManager>();
    memory_manager_ = std::make_unique<MemoryManager>();
    sync_manager_ = std::make_unique<SyncManager>();
    metrics_collector_ = std::make_unique<MetricsCollector>();
    
    // Default to FCFS scheduler
    scheduler_ = createScheduler(SchedulingAlgorithm::FCFS);
}

SimulationController::~SimulationController() {
    stopSimulation();
}

bool SimulationController::startSimulation() {
    std::lock_guard<std::mutex> lock(simulation_mutex_);
    
    if (state_ == SimulationState::RUNNING) {
        return true; // Already running
    }
    
    if (state_ == SimulationState::PAUSED) {
        // Resume from pause
        state_ = SimulationState::RUNNING;
        simulation_cv_.notify_one();
        return true;
    }
    
    // Start new simulation
    should_stop_ = false;
    state_ = step_mode_ ? SimulationState::STEP_MODE : SimulationState::RUNNING;
    
    if (!step_mode_) {
        simulation_thread_ = std::make_unique<std::thread>(&SimulationController::simulationLoop, this);
    }
    
    return true;
}

void SimulationController::pauseSimulation() {
    if (state_ == SimulationState::RUNNING) {
        state_ = SimulationState::PAUSED;
    }
}

void SimulationController::resumeSimulation() {
    std::lock_guard<std::mutex> lock(simulation_mutex_);
    
    if (state_ == SimulationState::PAUSED) {
        state_ = SimulationState::RUNNING;
        simulation_cv_.notify_one();
    }
}

void SimulationController::stopSimulation() {
    should_stop_ = true;
    state_ = SimulationState::STOPPED;
    
    {
        std::lock_guard<std::mutex> lock(simulation_mutex_);
        simulation_cv_.notify_all();
    }
    
    if (simulation_thread_ && simulation_thread_->joinable()) {
        simulation_thread_->join();
        simulation_thread_.reset();
    }
}

void SimulationController::resetSimulation() {
    stopSimulation();
    
    // Reset all components
    process_manager_->reset();
    memory_manager_->reset();
    sync_manager_->reset();
    metrics_collector_->reset();
    
    // Reset simulation state
    current_time_ = 0;
    current_running_process_ = -1;
    current_process_remaining_quantum_ = 0;
    
    state_ = SimulationState::STOPPED;
}

bool SimulationController::executeStep() {
    if (state_ != SimulationState::STEP_MODE && state_ != SimulationState::RUNNING) {
        return false;
    }
    
    // Process arrivals
    processArrivals();
    
    // Schedule process if needed
    if (current_running_process_ == -1) {
        scheduleProcess();
    }
    
    // Execute current process
    if (current_running_process_ != -1) {
        executeCurrentProcess();
    }
    
    // Update metrics
    updateMetrics();
    
    // Advance time
    current_time_++;
    
    return shouldContinueSimulation();
}

void SimulationController::setConfiguration(const SimulationConfig& config) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    config_ = config;
    
    // Apply configuration
    setSchedulingAlgorithm(config.scheduling_algo);
    setTimeQuantum(config.time_quantum);
    setSimulationSpeed(config.simulation_speed);
    setStepMode(config.step_mode);
    
    // Configure memory manager
    memory_manager_->setReplacementAlgorithm(config.memory_config.replacement_algo);
}

void SimulationController::setSimulationSpeed(int steps_per_second) {
    if (steps_per_second >= 1 && steps_per_second <= 10) {
        simulation_speed_ = steps_per_second;
    }
}

void SimulationController::setStepMode(bool enabled) {
    step_mode_ = enabled;
    
    if (enabled && state_ == SimulationState::RUNNING) {
        pauseSimulation();
        state_ = SimulationState::STEP_MODE;
    }
}

SystemMetrics SimulationController::getSystemMetrics() const {
    auto metrics = metrics_collector_->calculateMetrics();
    
    // Add memory statistics
    metrics.memory_utilization = memory_manager_->getMemoryUtilization();
    metrics.page_faults = memory_manager_->getPageFaultCount();
    metrics.page_replacements = memory_manager_->getPageReplacementCount();
    
    return metrics;
}

int SimulationController::addProcess(int priority, int burst_time, int arrival_time, int memory_pages) {
    // Validate parameters
    ValidationResult validation = InputValidator::validateProcessParameters(priority, burst_time, arrival_time);
    if (!validation.is_valid) {
        return -1;
    }
    
    // Create process
    int pid = process_manager_->createProcess(priority, burst_time, arrival_time);
    if (pid == -1) {
        return -1;
    }
    
    // Get process and record creation
    ProcessControlBlock* pcb = process_manager_->getProcess(pid);
    if (pcb) {
        metrics_collector_->recordProcessCreation(*pcb);
        
        // Allocate memory if needed
        if (memory_pages > 0) {
            memory_manager_->allocatePages(pid, memory_pages);
        }
    }
    
    return pid;
}

void SimulationController::setSchedulingAlgorithm(SchedulingAlgorithm algorithm) {
    scheduler_ = createScheduler(algorithm);
    config_.scheduling_algo = algorithm;
}

void SimulationController::setTimeQuantum(int quantum) {
    config_.time_quantum = quantum;
    
    // Update Round Robin scheduler if active
    if (config_.scheduling_algo == SchedulingAlgorithm::ROUND_ROBIN) {
        auto* rr_scheduler = dynamic_cast<RoundRobinScheduler*>(scheduler_.get());
        if (rr_scheduler) {
            rr_scheduler->setTimeQuantum(quantum);
        }
    }
}

void SimulationController::setMemoryReplacementAlgorithm(ReplacementAlgorithm algorithm) {
    memory_manager_->setReplacementAlgorithm(algorithm);
    config_.memory_config.replacement_algo = algorithm;
}

void SimulationController::simulationLoop() {
    while (!should_stop_) {
        std::unique_lock<std::mutex> lock(simulation_mutex_);
        
        // Wait if paused
        simulation_cv_.wait(lock, [this] {
            return should_stop_ || state_ == SimulationState::RUNNING;
        });
        
        if (should_stop_) {
            break;
        }
        
        lock.unlock();
        
        // Execute one step
        if (!executeStep()) {
            // Simulation completed naturally
            state_ = SimulationState::STOPPED;
            break;
        }
        
        // Sleep based on simulation speed
        sleepForTimeStep();
    }
}

void SimulationController::processArrivals() {
    // Check for processes that should arrive at current time
    std::lock_guard<std::mutex> lock(config_mutex_);
    
    for (const auto& process_config : config_.processes) {
        if (process_config.arrival_time == current_time_) {
            int pid = addProcess(process_config.priority, process_config.burst_time,
                               process_config.arrival_time, process_config.memory_requirement);
            
            if (pid != -1) {
                // Move process to ready state
                process_manager_->updateProcessState(pid, ProcessState::READY);
                metrics_collector_->recordStateChange(pid, ProcessState::NEW, ProcessState::READY, current_time_);
            }
        }
    }
}

void SimulationController::scheduleProcess() {
    // Get ready processes
    auto ready_processes = process_manager_->getProcessesByState(ProcessState::READY);
    
    if (ready_processes.empty()) {
        return; // No processes to schedule
    }
    
    // Select next process using current scheduler
    int selected_pid = scheduler_->selectNextProcess(ready_processes);
    
    if (selected_pid != -1) {
        current_running_process_ = selected_pid;
        
        // Update process state
        process_manager_->updateProcessState(selected_pid, ProcessState::RUNNING);
        metrics_collector_->recordStateChange(selected_pid, ProcessState::READY, ProcessState::RUNNING, current_time_);
        
        // Set start time if first execution
        ProcessControlBlock* pcb = process_manager_->getProcess(selected_pid);
        if (pcb && pcb->start_time == -1) {
            process_manager_->setProcessStartTime(selected_pid, current_time_);
        }
        
        // Initialize quantum for Round Robin
        if (config_.scheduling_algo == SchedulingAlgorithm::ROUND_ROBIN) {
            current_process_remaining_quantum_ = config_.time_quantum;
        }
    }
}

void SimulationController::executeCurrentProcess() {
    if (current_running_process_ == -1) {
        return;
    }
    
    // Execute process for one time unit
    bool has_remaining_time = process_manager_->decreaseRemainingTime(current_running_process_, 1);
    metrics_collector_->recordCPUExecution(1);
    
    // Update quantum for Round Robin
    if (config_.scheduling_algo == SchedulingAlgorithm::ROUND_ROBIN) {
        current_process_remaining_quantum_--;
    }
    
    // Check if process completed
    if (!has_remaining_time) {
        handleProcessCompletion(current_running_process_);
        return;
    }
    
    // Check for preemption
    bool should_preempt = false;
    
    if (config_.scheduling_algo == SchedulingAlgorithm::ROUND_ROBIN) {
        // Time quantum expired
        if (current_process_remaining_quantum_ <= 0) {
            should_preempt = true;
        }
    }
    
    if (should_preempt) {
        // Move current process back to ready
        process_manager_->updateProcessState(current_running_process_, ProcessState::READY);
        metrics_collector_->recordStateChange(current_running_process_, ProcessState::RUNNING, ProcessState::READY, current_time_);
        
        current_running_process_ = -1;
        current_process_remaining_quantum_ = 0;
    }
}

void SimulationController::handleProcessCompletion(int pid) {
    // Set completion time
    process_manager_->setProcessCompletionTime(pid, current_time_);
    
    // Update state to terminated
    process_manager_->updateProcessState(pid, ProcessState::TERMINATED);
    metrics_collector_->recordStateChange(pid, ProcessState::RUNNING, ProcessState::TERMINATED, current_time_);
    
    // Record completion in metrics
    ProcessControlBlock* pcb = process_manager_->getProcess(pid);
    if (pcb) {
        metrics_collector_->recordProcessCompletion(*pcb, current_time_);
    }
    
    // Deallocate memory
    memory_manager_->deallocatePages(pid);
    
    // Clean up synchronization resources
    sync_manager_->cleanupProcess(pid);
    
    // Terminate process
    process_manager_->terminateProcess(pid);
    
    current_running_process_ = -1;
    current_process_remaining_quantum_ = 0;
}

void SimulationController::updateMetrics() {
    metrics_collector_->updateSimulationTime(current_time_);
    
    // Update memory statistics
    metrics_collector_->recordMemoryStats(
        memory_manager_->getPageFaultCount(),
        memory_manager_->getPageReplacementCount(),
        memory_manager_->getMemoryUtilization()
    );
}

std::unique_ptr<Scheduler> SimulationController::createScheduler(SchedulingAlgorithm algorithm) {
    switch (algorithm) {
        case SchedulingAlgorithm::FCFS:
            return std::make_unique<FCFSScheduler>();
        case SchedulingAlgorithm::ROUND_ROBIN:
            return std::make_unique<RoundRobinScheduler>(config_.time_quantum);
        case SchedulingAlgorithm::PRIORITY:
            return std::make_unique<PriorityScheduler>();
        default:
            return std::make_unique<FCFSScheduler>();
    }
}

bool SimulationController::shouldContinueSimulation() const {
    // Continue if there are active processes or processes yet to arrive
    size_t active_processes = process_manager_->getProcessCount();
    
    // Check for future arrivals
    bool has_future_arrivals = false;
    {
        std::lock_guard<std::mutex> lock(config_mutex_);
        for (const auto& process_config : config_.processes) {
            if (process_config.arrival_time > current_time_) {
                has_future_arrivals = true;
                break;
            }
        }
    }
    
    return active_processes > 0 || has_future_arrivals;
}

void SimulationController::sleepForTimeStep() {
    if (simulation_speed_ > 0) {
        int delay_ms = 1000 / simulation_speed_;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
}