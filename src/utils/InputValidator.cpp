#include "InputValidator.h"
#include <cmath>

ValidationResult InputValidator::validateProcessParameters(int priority, int burst_time, int arrival_time) {
    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
        return ValidationResult(false, "Priority must be between " + 
                               std::to_string(MIN_PRIORITY) + " and " + 
                               std::to_string(MAX_PRIORITY));
    }
    
    if (burst_time <= 0) {
        return ValidationResult(false, "Burst time must be positive");
    }
    
    if (arrival_time < 0) {
        return ValidationResult(false, "Arrival time cannot be negative");
    }
    
    return ValidationResult(true);
}

ValidationResult InputValidator::validateMemoryConfiguration(int total_memory, int page_size) {
    if (total_memory <= 0) {
        return ValidationResult(false, "Total memory size must be positive");
    }
    
    if (page_size <= 0) {
        return ValidationResult(false, "Page size must be positive");
    }
    
    // Check if page_size is power of 2
    if ((page_size & (page_size - 1)) != 0) {
        return ValidationResult(false, "Page size must be a power of 2");
    }
    
    if (total_memory % page_size != 0) {
        return ValidationResult(false, "Total memory size must be divisible by page size");
    }
    
    return ValidationResult(true);
}

ValidationResult InputValidator::validateTimeQuantum(int time_quantum) {
    if (time_quantum <= 0) {
        return ValidationResult(false, "Time quantum must be positive");
    }
    
    return ValidationResult(true);
}

ValidationResult InputValidator::validateSimulationSpeed(int speed) {
    if (speed <= 0) {
        return ValidationResult(false, "Simulation speed must be positive");
    }
    
    if (speed > MAX_SIMULATION_SPEED) {
        return ValidationResult(false, "Simulation speed cannot exceed " + 
                               std::to_string(MAX_SIMULATION_SPEED) + " steps per second");
    }
    
    return ValidationResult(true);
}

ValidationResult InputValidator::validateProcessCount(int count) {
    if (count <= 0) {
        return ValidationResult(false, "Process count must be positive");
    }
    
    if (count > MAX_PROCESSES) {
        return ValidationResult(false, "Process count cannot exceed " + 
                               std::to_string(MAX_PROCESSES) + " processes");
    }
    
    return ValidationResult(true);
}