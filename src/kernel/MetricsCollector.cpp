#include "MetricsCollector.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

MetricsCollector::MetricsCollector()
    : current_simulation_time_(0), total_cpu_execution_time_(0),
      total_processes_(0), completed_processes_count_(0),
      total_page_faults_(0), total_page_replacements_(0),
      current_memory_utilization_(0.0) {
}

void MetricsCollector::recordProcessCreation(const ProcessControlBlock& process) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    process_info_.emplace(process.pid, ProcessTrackingInfo(process));
    total_processes_++;
}

void MetricsCollector::recordProcessCompletion(const ProcessControlBlock& process, int completion_time) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    auto it = process_info_.find(process.pid);
    if (it == process_info_.end()) {
        return; // Process not found
    }
    
    ProcessTrackingInfo& info = it->second;
    info.pcb.completion_time = completion_time;
    
    // Calculate final metrics for this process
    ProcessMetrics metrics;
    metrics.waiting_time = calculateWaitingTime(info);
    metrics.turnaround_time = calculateTurnaroundTime(info);
    metrics.response_time = calculateResponseTime(info);
    metrics.completion_time = completion_time;
    
    // Store completed process metrics
    completed_processes_[process.pid] = metrics;
    completed_processes_count_++;
    
    // Remove from active tracking
    process_info_.erase(it);
}

void MetricsCollector::recordStateChange(int pid, ProcessState old_state, ProcessState new_state, int current_time) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    auto it = process_info_.find(pid);
    if (it == process_info_.end()) {
        return; // Process not found
    }
    
    ProcessTrackingInfo& info = it->second;
    
    // Update waiting time if process was in READY state
    if (old_state == ProcessState::READY) {
        int wait_time = current_time - info.last_state_change_time;
        info.total_wait_time += wait_time;
    }
    
    // Record first run time for response time calculation
    if (new_state == ProcessState::RUNNING && !info.has_started) {
        info.first_run_time = current_time;
        info.has_started = true;
    }
    
    info.last_state_change_time = current_time;
    info.pcb.state = new_state;
}

void MetricsCollector::updateSimulationTime(int current_time) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    current_simulation_time_ = current_time;
}

void MetricsCollector::recordCPUExecution(int time_units) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    total_cpu_execution_time_ += time_units;
}

void MetricsCollector::recordMemoryStats(int page_faults, int page_replacements, double memory_utilization) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    total_page_faults_ = page_faults;
    total_page_replacements_ = page_replacements;
    current_memory_utilization_ = memory_utilization;
}

SystemMetrics MetricsCollector::calculateMetrics() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    SystemMetrics metrics;
    
    // Basic counts
    metrics.total_processes = total_processes_;
    metrics.completed_processes = completed_processes_count_;
    metrics.current_time = current_simulation_time_;
    
    // Calculate averages from completed processes
    if (completed_processes_count_ > 0) {
        double total_waiting_time = 0.0;
        double total_turnaround_time = 0.0;
        double total_response_time = 0.0;
        
        for (const auto& pair : completed_processes_) {
            const ProcessMetrics& pm = pair.second;
            total_waiting_time += pm.waiting_time;
            total_turnaround_time += pm.turnaround_time;
            total_response_time += pm.response_time;
        }
        
        metrics.average_waiting_time = total_waiting_time / completed_processes_count_;
        metrics.average_turnaround_time = total_turnaround_time / completed_processes_count_;
        metrics.average_response_time = total_response_time / completed_processes_count_;
    }
    
    // CPU utilization
    if (current_simulation_time_ > 0) {
        metrics.cpu_utilization = (static_cast<double>(total_cpu_execution_time_) / current_simulation_time_) * 100.0;
        metrics.throughput = static_cast<double>(completed_processes_count_) / current_simulation_time_;
    }
    
    // Memory metrics
    metrics.memory_utilization = current_memory_utilization_;
    metrics.page_faults = total_page_faults_;
    metrics.page_replacements = total_page_replacements_;
    
    return metrics;
}

ProcessMetrics MetricsCollector::getProcessMetrics(int pid) const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    auto it = completed_processes_.find(pid);
    if (it != completed_processes_.end()) {
        return it->second;
    }
    
    // Check if process is still active
    auto active_it = process_info_.find(pid);
    if (active_it != process_info_.end()) {
        const ProcessTrackingInfo& info = active_it->second;
        ProcessMetrics metrics;
        metrics.waiting_time = calculateWaitingTime(info);
        metrics.turnaround_time = 0; // Not completed yet
        metrics.response_time = calculateResponseTime(info);
        metrics.completion_time = -1; // Not completed yet
        return metrics;
    }
    
    return ProcessMetrics(); // Default metrics
}

std::unordered_map<int, ProcessMetrics> MetricsCollector::getAllProcessMetrics() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    return completed_processes_;
}

void MetricsCollector::reset() {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    process_info_.clear();
    completed_processes_.clear();
    
    current_simulation_time_ = 0;
    total_cpu_execution_time_ = 0;
    total_processes_ = 0;
    completed_processes_count_ = 0;
    total_page_faults_ = 0;
    total_page_replacements_ = 0;
    current_memory_utilization_ = 0.0;
}

std::string MetricsCollector::generateReport() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    std::ostringstream report;
    SystemMetrics metrics = calculateMetrics();
    
    report << "=== Mini OS Kernel Simulator Performance Report ===\n\n";
    
    // Process Statistics
    report << "Process Statistics:\n";
    report << "  Total Processes: " << metrics.total_processes << "\n";
    report << "  Completed Processes: " << metrics.completed_processes << "\n";
    report << "  Simulation Time: " << metrics.current_time << " time units\n\n";
    
    // Timing Metrics
    report << "Timing Metrics:\n";
    report << std::fixed << std::setprecision(2);
    report << "  Average Waiting Time: " << metrics.average_waiting_time << " time units\n";
    report << "  Average Turnaround Time: " << metrics.average_turnaround_time << " time units\n";
    report << "  Average Response Time: " << metrics.average_response_time << " time units\n\n";
    
    // System Performance
    report << "System Performance:\n";
    report << "  CPU Utilization: " << metrics.cpu_utilization << "%\n";
    report << "  Throughput: " << metrics.throughput << " processes/time unit\n\n";
    
    // Memory Statistics
    report << "Memory Statistics:\n";
    report << "  Memory Utilization: " << metrics.memory_utilization << "%\n";
    report << "  Total Page Faults: " << metrics.page_faults << "\n";
    report << "  Total Page Replacements: " << metrics.page_replacements << "\n\n";
    
    // Individual Process Details
    if (!completed_processes_.empty()) {
        report << "Individual Process Metrics:\n";
        report << "  PID | Waiting | Turnaround | Response | Completion\n";
        report << "  ----|---------|------------|----------|------------\n";
        
        for (const auto& pair : completed_processes_) {
            const ProcessMetrics& pm = pair.second;
            report << std::setw(5) << pair.first << " |";
            report << std::setw(8) << pm.waiting_time << " |";
            report << std::setw(11) << pm.turnaround_time << " |";
            report << std::setw(9) << pm.response_time << " |";
            report << std::setw(11) << pm.completion_time << "\n";
        }
    }
    
    return report.str();
}

double MetricsCollector::getAverageWaitingTime() const {
    return calculateMetrics().average_waiting_time;
}

double MetricsCollector::getAverageTurnaroundTime() const {
    return calculateMetrics().average_turnaround_time;
}

double MetricsCollector::getAverageResponseTime() const {
    return calculateMetrics().average_response_time;
}

double MetricsCollector::getCPUUtilization() const {
    return calculateMetrics().cpu_utilization;
}

double MetricsCollector::getThroughput() const {
    return calculateMetrics().throughput;
}

int MetricsCollector::getTotalProcesses() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    return total_processes_;
}

int MetricsCollector::getCompletedProcesses() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    return completed_processes_count_;
}

int MetricsCollector::getCurrentTime() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    return current_simulation_time_;
}

int MetricsCollector::calculateWaitingTime(const ProcessTrackingInfo& info) const {
    // Waiting time = total time in READY state
    return info.total_wait_time;
}

int MetricsCollector::calculateTurnaroundTime(const ProcessTrackingInfo& info) const {
    // Turnaround time = completion time - arrival time
    if (info.pcb.completion_time >= 0) {
        return info.pcb.completion_time - info.pcb.arrival_time;
    }
    return 0;
}

int MetricsCollector::calculateResponseTime(const ProcessTrackingInfo& info) const {
    // Response time = first run time - arrival time
    if (info.first_run_time >= 0) {
        return info.first_run_time - info.pcb.arrival_time;
    }
    return 0;
}

void MetricsCollector::updateWaitingTime(int pid, int current_time) {
    auto it = process_info_.find(pid);
    if (it != process_info_.end()) {
        ProcessTrackingInfo& info = it->second;
        if (info.pcb.state == ProcessState::READY) {
            int additional_wait = current_time - info.last_state_change_time;
            info.total_wait_time += additional_wait;
            info.last_state_change_time = current_time;
        }
    }
}