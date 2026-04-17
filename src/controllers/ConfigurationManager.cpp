#include "ConfigurationManager.h"
#include "InputValidator.h"
#include <fstream>
#include <sstream>
#include <iomanip>

bool ConfigurationManager::saveConfiguration(const SimulationConfig& config, const std::string& filename) {
    if (!validateConfiguration(config)) {
        return false;
    }
    
    try {
        std::string json_content = configurationToJson(config);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            setError("Failed to open file for writing: " + filename);
            return false;
        }
        
        file << json_content;
        file.close();
        
        if (file.fail()) {
            setError("Failed to write to file: " + filename);
            return false;
        }
        
        return true;
    }
    catch (const std::exception& e) {
        setError("Exception while saving configuration: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationManager::loadConfiguration(const std::string& filename, SimulationConfig& config) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            setError("Failed to open file for reading: " + filename);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        std::string json_content = buffer.str();
        
        if (!configurationFromJson(json_content, config)) {
            return false;
        }
        
        if (!validateConfiguration(config)) {
            return false;
        }
        
        return true;
    }
    catch (const std::exception& e) {
        setError("Exception while loading configuration: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationManager::validateConfiguration(const SimulationConfig& config) {
    // Validate simulation speed
    auto speed_result = InputValidator::validateSimulationSpeed(config.simulation_speed);
    if (!speed_result.is_valid) {
        setError("Invalid simulation speed: " + speed_result.error_message);
        return false;
    }
    
    // Validate time quantum
    if (config.scheduling_algo == SchedulingAlgorithm::ROUND_ROBIN) {
        auto quantum_result = InputValidator::validateTimeQuantum(config.time_quantum);
        if (!quantum_result.is_valid) {
            setError("Invalid time quantum: " + quantum_result.error_message);
            return false;
        }
    }
    
    // Validate memory configuration
    auto memory_result = InputValidator::validateMemoryConfiguration(
        config.memory_config.total_memory_size,
        config.memory_config.page_size
    );
    if (!memory_result.is_valid) {
        setError("Invalid memory configuration: " + memory_result.error_message);
        return false;
    }
    
    // Validate process count
    auto count_result = InputValidator::validateProcessCount(static_cast<int>(config.processes.size()));
    if (!count_result.is_valid) {
        setError("Invalid process count: " + count_result.error_message);
        return false;
    }
    
    // Validate each process
    for (const auto& process : config.processes) {
        auto process_result = InputValidator::validateProcessParameters(
            process.priority, process.burst_time, process.arrival_time
        );
        if (!process_result.is_valid) {
            setError("Invalid process parameters: " + process_result.error_message);
            return false;
        }
    }
    
    return true;
}

std::string ConfigurationManager::configurationToJson(const SimulationConfig& config) {
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);
    
    json << "{\n";
    json << "  \"version\": \"1.0\",\n";
    json << "  \"timestamp\": \"" << std::time(nullptr) << "\",\n";
    json << "  \"scheduling_algorithm\": \"" << schedulingAlgorithmToString(config.scheduling_algo) << "\",\n";
    json << "  \"time_quantum\": " << config.time_quantum << ",\n";
    json << "  \"simulation_speed\": " << config.simulation_speed << ",\n";
    json << "  \"step_mode\": " << (config.step_mode ? "true" : "false") << ",\n";
    
    // Memory configuration
    json << "  \"memory_config\": {\n";
    json << "    \"total_memory_size\": " << config.memory_config.total_memory_size << ",\n";
    json << "    \"page_size\": " << config.memory_config.page_size << ",\n";
    json << "    \"replacement_algorithm\": \"" << replacementAlgorithmToString(config.memory_config.replacement_algo) << "\"\n";
    json << "  },\n";
    
    // Processes
    json << "  \"processes\": [\n";
    for (size_t i = 0; i < config.processes.size(); ++i) {
        const auto& process = config.processes[i];
        json << "    {\n";
        json << "      \"priority\": " << process.priority << ",\n";
        json << "      \"burst_time\": " << process.burst_time << ",\n";
        json << "      \"arrival_time\": " << process.arrival_time << ",\n";
        json << "      \"memory_requirement\": " << process.memory_requirement << "\n";
        json << "    }";
        if (i < config.processes.size() - 1) {
            json << ",";
        }
        json << "\n";
    }
    json << "  ]\n";
    json << "}\n";
    
    return json.str();
}

bool ConfigurationManager::configurationFromJson(const std::string& json_string, SimulationConfig& config) {
    // Simple JSON parsing (in a real implementation, you'd use a JSON library like nlohmann/json)
    // This is a simplified parser for demonstration
    
    try {
        // Reset configuration to defaults
        config = createDefaultConfiguration();
        
        // Parse key-value pairs (simplified parsing)
        std::istringstream stream(json_string);
        std::string line;
        
        bool in_memory_config = false;
        bool in_processes = false;
        ProcessConfig current_process;
        
        while (std::getline(stream, line)) {
            // Remove whitespace and quotes
            size_t start = line.find_first_not_of(" \t\"");
            if (start == std::string::npos) continue;
            
            line = line.substr(start);
            
            if (line.find("memory_config") != std::string::npos) {
                in_memory_config = true;
                continue;
            } else if (line.find("processes") != std::string::npos) {
                in_processes = true;
                continue;
            } else if (line.find("}") != std::string::npos) {
                if (in_memory_config) {
                    in_memory_config = false;
                } else if (in_processes) {
                    in_processes = false;
                }
                continue;
            }
            
            // Parse key-value pairs
            size_t colon_pos = line.find(':');
            if (colon_pos == std::string::npos) continue;
            
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);
            
            // Remove quotes and whitespace
            key.erase(0, key.find_first_not_of(" \t\""));
            key.erase(key.find_last_not_of(" \t\",") + 1);
            
            value.erase(0, value.find_first_not_of(" \t\""));
            value.erase(value.find_last_not_of(" \t\",") + 1);
            
            // Parse values based on context
            if (in_memory_config) {
                if (key == "total_memory_size") {
                    config.memory_config.total_memory_size = std::stoi(value);
                } else if (key == "page_size") {
                    config.memory_config.page_size = std::stoi(value);
                    config.memory_config.num_frames = config.memory_config.total_memory_size / config.memory_config.page_size;
                } else if (key == "replacement_algorithm") {
                    config.memory_config.replacement_algo = stringToReplacementAlgorithm(value);
                }
            } else if (in_processes) {
                if (key == "priority") {
                    current_process.priority = std::stoi(value);
                } else if (key == "burst_time") {
                    current_process.burst_time = std::stoi(value);
                } else if (key == "arrival_time") {
                    current_process.arrival_time = std::stoi(value);
                } else if (key == "memory_requirement") {
                    current_process.memory_requirement = std::stoi(value);
                    // Only push when we have a complete process (all 4 fields)
                    if (current_process.burst_time > 0) {
                        config.processes.push_back(current_process);
                        current_process = ProcessConfig(); // Reset for next process
                    }
                }
            } else {
                // Main configuration
                if (key == "scheduling_algorithm") {
                    config.scheduling_algo = stringToSchedulingAlgorithm(value);
                } else if (key == "time_quantum") {
                    config.time_quantum = std::stoi(value);
                } else if (key == "simulation_speed") {
                    config.simulation_speed = std::stoi(value);
                } else if (key == "step_mode") {
                    config.step_mode = (value == "true");
                }
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        setError("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

SimulationConfig ConfigurationManager::createDefaultConfiguration() {
    SimulationConfig config;
    
    // Default scheduling
    config.scheduling_algo = SchedulingAlgorithm::FCFS;
    config.time_quantum = 4;
    
    // Default memory configuration
    config.memory_config = MemoryConfiguration(256, 32, ReplacementAlgorithm::LRU);
    
    // Default simulation settings
    config.simulation_speed = 1;
    config.step_mode = false;
    
    // Default processes
    config.processes.clear();
    config.processes.emplace_back(5, 10, 0, 2);  // Priority 5, burst 10, arrival 0, memory 2
    config.processes.emplace_back(3, 8, 2, 1);   // Priority 3, burst 8, arrival 2, memory 1
    config.processes.emplace_back(7, 6, 4, 3);   // Priority 7, burst 6, arrival 4, memory 3
    
    return config;
}

std::string ConfigurationManager::schedulingAlgorithmToString(SchedulingAlgorithm algorithm) {
    switch (algorithm) {
        case SchedulingAlgorithm::FCFS:
            return "FCFS";
        case SchedulingAlgorithm::ROUND_ROBIN:
            return "ROUND_ROBIN";
        case SchedulingAlgorithm::PRIORITY:
            return "PRIORITY";
        default:
            return "FCFS";
    }
}

SchedulingAlgorithm ConfigurationManager::stringToSchedulingAlgorithm(const std::string& str) {
    if (str == "ROUND_ROBIN") {
        return SchedulingAlgorithm::ROUND_ROBIN;
    } else if (str == "PRIORITY") {
        return SchedulingAlgorithm::PRIORITY;
    } else {
        return SchedulingAlgorithm::FCFS;
    }
}

std::string ConfigurationManager::replacementAlgorithmToString(ReplacementAlgorithm algorithm) {
    switch (algorithm) {
        case ReplacementAlgorithm::FIFO:
            return "FIFO";
        case ReplacementAlgorithm::LRU:
            return "LRU";
        default:
            return "LRU";
    }
}

ReplacementAlgorithm ConfigurationManager::stringToReplacementAlgorithm(const std::string& str) {
    if (str == "FIFO") {
        return ReplacementAlgorithm::FIFO;
    } else {
        return ReplacementAlgorithm::LRU;
    }
}

void ConfigurationManager::setError(const std::string& error) {
    last_error_ = error;
}