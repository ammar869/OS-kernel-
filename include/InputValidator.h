#pragma once

#include <string>

/**
 * @brief Result of input validation
 */
struct ValidationResult {
    bool is_valid;          ///< Whether the input is valid
    std::string error_message;  ///< Error message if validation failed
    
    ValidationResult(bool valid = true, const std::string& message = "")
        : is_valid(valid), error_message(message) {}
};

/**
 * @brief Input validation utility class
 * 
 * Provides static methods for validating various input parameters
 * used throughout the simulation system.
 */
class InputValidator {
public:
    /**
     * @brief Validate process parameters
     * @param priority Process priority (should be 1-10)
     * @param burst_time Process burst time (should be positive)
     * @param arrival_time Process arrival time (should be non-negative)
     * @return ValidationResult indicating success or failure with error message
     */
    static ValidationResult validateProcessParameters(int priority, int burst_time, int arrival_time);
    
    /**
     * @brief Validate memory configuration parameters
     * @param total_memory Total memory size (should be positive)
     * @param page_size Page size (should be positive and power of 2)
     * @return ValidationResult indicating success or failure with error message
     */
    static ValidationResult validateMemoryConfiguration(int total_memory, int page_size);
    
    /**
     * @brief Validate time quantum for Round Robin scheduling
     * @param time_quantum Time quantum (should be positive)
     * @return ValidationResult indicating success or failure with error message
     */
    static ValidationResult validateTimeQuantum(int time_quantum);
    
    /**
     * @brief Validate simulation speed
     * @param speed Simulation speed in steps per second (should be positive)
     * @return ValidationResult indicating success or failure with error message
     */
    static ValidationResult validateSimulationSpeed(int speed);
    
    /**
     * @brief Validate process count
     * @param count Number of processes (should be positive and within limits)
     * @return ValidationResult indicating success or failure with error message
     */
    static ValidationResult validateProcessCount(int count);

private:
    static constexpr int MAX_PROCESSES = 100;
    static constexpr int MIN_PRIORITY = 1;
    static constexpr int MAX_PRIORITY = 10;
    static constexpr int MAX_SIMULATION_SPEED = 10;
};