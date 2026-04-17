#pragma once

#include "SimulationConfig.h"
#include <string>
#include <memory>

/**
 * @brief Manages simulation configuration save/load operations
 * 
 * The ConfigurationManager handles serialization and deserialization of
 * simulation configurations to/from JSON files.
 */
class ConfigurationManager {
public:
    /**
     * @brief Constructor
     */
    ConfigurationManager() = default;
    
    /**
     * @brief Destructor
     */
    ~ConfigurationManager() = default;
    
    /**
     * @brief Save configuration to file
     * @param config Configuration to save
     * @param filename File path to save to
     * @return true if save successful
     */
    bool saveConfiguration(const SimulationConfig& config, const std::string& filename);
    
    /**
     * @brief Load configuration from file
     * @param filename File path to load from
     * @param config Configuration object to populate
     * @return true if load successful
     */
    bool loadConfiguration(const std::string& filename, SimulationConfig& config);
    
    /**
     * @brief Validate configuration parameters
     * @param config Configuration to validate
     * @return true if configuration is valid
     */
    bool validateConfiguration(const SimulationConfig& config);
    
    /**
     * @brief Get last error message
     * @return Error message from last operation
     */
    const std::string& getLastError() const { return last_error_; }
    
    /**
     * @brief Convert configuration to JSON string
     * @param config Configuration to convert
     * @return JSON string representation
     */
    std::string configurationToJson(const SimulationConfig& config);
    
    /**
     * @brief Parse configuration from JSON string
     * @param json_string JSON string to parse
     * @param config Configuration object to populate
     * @return true if parsing successful
     */
    bool configurationFromJson(const std::string& json_string, SimulationConfig& config);
    
    /**
     * @brief Create default configuration
     * @return Default simulation configuration
     */
    static SimulationConfig createDefaultConfiguration();

private:
    std::string last_error_;
    
    /**
     * @brief Convert scheduling algorithm enum to string
     * @param algorithm Scheduling algorithm
     * @return String representation
     */
    std::string schedulingAlgorithmToString(SchedulingAlgorithm algorithm);
    
    /**
     * @brief Convert string to scheduling algorithm enum
     * @param str String representation
     * @return Scheduling algorithm enum
     */
    SchedulingAlgorithm stringToSchedulingAlgorithm(const std::string& str);
    
    /**
     * @brief Convert replacement algorithm enum to string
     * @param algorithm Replacement algorithm
     * @return String representation
     */
    std::string replacementAlgorithmToString(ReplacementAlgorithm algorithm);
    
    /**
     * @brief Convert string to replacement algorithm enum
     * @param str String representation
     * @return Replacement algorithm enum
     */
    ReplacementAlgorithm stringToReplacementAlgorithm(const std::string& str);
    
    /**
     * @brief Set error message
     * @param error Error message
     */
    void setError(const std::string& error);
};