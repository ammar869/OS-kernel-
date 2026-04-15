#pragma once

#include "ProcessState.h"
#include "MemoryTypes.h"
#include <vector>

/**
 * @brief Configuration for a single process in the simulation
 */
struct ProcessConfig {
    int priority;           ///< Process priority (1-10)
    int burst_time;         ///< CPU burst time required
    int arrival_time;       ///< Time when process arrives
    int memory_requirement; ///< Number of memory pages required
    
    ProcessConfig(int prio = 5, int burst = 10, int arrival = 0, int memory = 1)
        : priority(prio), burst_time(burst), arrival_time(arrival), 
          memory_requirement(memory) {}
};

/**
 * @brief Complete simulation configuration
 */
struct SimulationConfig {
    std::vector<ProcessConfig> processes;        ///< List of processes to simulate
    SchedulingAlgorithm scheduling_algo;         ///< CPU scheduling algorithm
    int time_quantum;                           ///< Time quantum for Round Robin
    MemoryConfiguration memory_config;          ///< Memory management configuration
    int simulation_speed;                       ///< Steps per second
    bool step_mode;                            ///< Whether to run in step-by-step mode
    
    SimulationConfig() 
        : scheduling_algo(SchedulingAlgorithm::FCFS), time_quantum(4),
          simulation_speed(1), step_mode(false) {}
};

/**
 * @brief System performance metrics
 */
struct SystemMetrics {
    double average_waiting_time;      ///< Average waiting time across all processes
    double average_turnaround_time;   ///< Average turnaround time
    double average_response_time;     ///< Average response time
    double cpu_utilization;           ///< CPU utilization percentage
    double throughput;                ///< Processes completed per time unit
    int total_processes;              ///< Total number of processes
    int completed_processes;          ///< Number of completed processes
    int current_time;                 ///< Current simulation time
    
    // Memory metrics
    double memory_utilization;        ///< Memory utilization percentage
    int page_faults;                  ///< Total number of page faults
    int page_replacements;            ///< Total number of page replacements
    
    SystemMetrics() : average_waiting_time(0.0), average_turnaround_time(0.0),
                     average_response_time(0.0), cpu_utilization(0.0),
                     throughput(0.0), total_processes(0), completed_processes(0),
                     current_time(0), memory_utilization(0.0), page_faults(0),
                     page_replacements(0) {}
};