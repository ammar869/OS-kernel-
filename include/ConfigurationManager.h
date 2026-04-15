#pragma once

#include "SimulationConfig.h"
#include <string>
#include <vector>

/**
 * @brief Manages simulation configuration save/load operations
 * 
 * The ConfigurationManager handles serialization and deserialization of
 * simulation configurations to/from JSON files.
 */
class ConfigurationManager {
public:
    /**
     * @brief Save configuration to file
     * @param config Configuration to save
     * @param filename File path to save to
     * @return true if saved successfully
     */
    static bool saveConfiguration(const SimulationConfig& config, const std::string& filename);
    
    /**
     * @brief Load configuration from file
     * @param filename File path to load from
     * @param config Configuration object to populate
     * @return true if loaded successfully
     */
    static bool loadConfiguration(const std::string& filename, SimulationConfig& config);
    
    /**
     * @brief Validate configuration parameters
     * @param config Configuration to validate
     * @return Validation result with error message if invalid
     */
    static ValidationResult validateConfiguration(const SimulationConfig& config);
    
    /**
     * @brief Get default configuration
     * @return Default simulation configuration
     */
    static SimulationConfig getDefaultConfiguration();
    
    /**
     * @brief Create sample configuration with predefined processes
     * @return Sample configuration for demonstration
     */
    static SimulationConfig createSampleConfiguration();
    
    /**
     * @brief Convert configuration to JSON string
     * @param config Configuration to convert
     * @return JSON string representation
     */
    static std::string configurationToJson(const SimulationConfig& config);
    
    /**
     * @brief Parse configuration from JSON string
     * @param json_string JSON string to parse
     * @param config Configuration object to populate
     * @return true if parsed successfully
     */
    static bool configurationFromJson(const std::string& json_string, SimulationConfig& config);

private:
    /**
     * @brief Convert scheduling algorithm to string
     * @param algorithm Scheduling algorithm
     * @return String representation
     */
    static std::string schedulingAlgorithmToString(SchedulingAlgorithm algorithm);
    
    /**
     * @brief Parse scheduling algorithm from string
     * @param str String representation
     * @return Scheduling algorithm
     */
    static SchedulingAlgorithm schedulingAlgorithmFromString(const std::string& str);
    
    /**
     * @brief Convert replacement algorithm to string
     * @param algorithm Replacement algorithm
     * @return String representation
     */
    static std::string replacementAlgorithmToString(ReplacementAlgorithm algorithm);
    
    /**
     * @brief Parse replacement algorithm from string
     * @param str String representation
     * @return Replacement algorithm
     */
    static ReplacementAlgorithm replacementAlgorithmFromString(const std::string& str);
    
    /**
     * @brief Read file contents
     * @param filename File to read
     * @return File contents as string, empty if failed
     */
    static std::string readFile(const std::string& filename);
    
    /**
     * @brief Write string to file
     * @param filename File to write to
     * @param content Content to write
     * @return true if written successfully
     */
    static bool writeFile(const std::string& filename, const std::string& content);
};