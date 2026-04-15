#include "ConfigurationManager.h"
#include "InputValidator.h"
#include <fstream>
#include <sstream>
#include <iomanip>

bool ConfigurationManager::saveConfiguration(const SimulationConfig& config, const std::string& filename) {
    // Validate configuration first
    ValidationResult validation = validateConfiguration(config);
    if (!validation.is_valid) {
        return false;
    }
    
    std::string json_content = configurationToJson(config);
    return writeFile(filename, json_content);
}

bool ConfigurationManager::loadConfiguration(const std::string& filename, SimulationConfig& config) {
    std::string json_content = readFile(filename);
    if (json_content.empty()) {
        return false;
    }
    
    return configurationFromJson(json_content, config);
}

ValidationResult ConfigurationManager::validateConfiguration(const SimulationConfig& config) {
    // Validate time quantum
    if (config.scheduling_algo == SchedulingAlgorithm::ROUND_ROBIN) {
        ValidationResult quantum_result = InputValidator::validateTimeQuantum(config.time_quantum);
        if (!quantum_result.is_valid) {
            return quantum_result;
        }
    }
    
    // Validate simulation speed
    ValidationResult speed_result = InputValidator::validateSimulationSpeed(config.simulation_speed);
    if (!speed_result.is_valid) {
        return speed_result;
    }
    
    // Validate memory configuration
    ValidationResult memory_result = InputValidator::validateMemoryConfiguration(
        config.memory_config.total_memory_size, config.memory_config.page_size);
    if (!memory_result.is_valid) {
        return memory_result;
    }
    
    // Validate process count
    ValidationResult count_result = InputValidator::validateProcessCount(static_cast<int>(config.processes.size()));
    if (!count_result.is_valid) {
        return count_result;
    }
    
    // Validate individual processes
    for (const auto& process : config.processes) {
        ValidationResult process_result = InputValidator::validateProcessParameters(
            process.priority, process.burst_time, process.arrival_time);
        if (!process_result.is_valid) {
            return ValidationResult(false, "Process validation failed: " + process_result.error_message);
        }
    }
    
    return ValidationResult(true);
}

SimulationConfig ConfigurationManager::getDefaultConfiguration() {
    SimulationConfig config;
    
    // Default scheduling
    config.scheduling_algo = SchedulingAlgorithm::FCFS;
    config.time_quantum = 4;
    
    // Default memory configuration
    config.memory_config = MemoryConfiguration(256, 32, ReplacementAlgorithm::LRU);
    
    // Default simulation settings
    config.simulation_speed = 1;
    config.step_mode = false;
    
    // No processes by default
    config.processes.clear();
    
    return config;
}

SimulationConfig ConfigurationManager::createSampleConfiguration() {
    SimulationConfig config = getDefaultConfiguration();
    
    // Add sample processes
    config.processes.push_back(ProcessConfig(5, 10, 0, 2));  // P1: priority=5, burst=10, arrival=0, memory=2
    config.processes.push_back(ProcessConfig(3, 8, 2, 1));   // P2: priority=3, burst=8, arrival=2, memory=1
    config.processes.push_back(ProcessConfig(7, 6, 4, 3));   // P3: priority=7, burst=6, arrival=4, memory=3
    config.processes.push_back(ProcessConfig(2, 12, 6, 2));  // P4: priority=2, burst=12, arrival=6, memory=2
    
    // Use Round Robin scheduling
    config.scheduling_algo = SchedulingAlgorithm::ROUND_ROBIN;
    config.time_quantum = 4;
    
    return config;
}

std::string ConfigurationManager::configurationToJson(const SimulationConfig& config) {
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);
    
    json << "{\n";
    json << "  \"version\": \"1.0\",\n";
    json << "  \"scheduling\": {\n";
    json << "    \"algorithm\": \"" << schedulingAlgorithmToString(config.scheduling_algo) << "\",\n";
    json << "    \"time_quantum\": " << config.time_quantum << "\n";
    json << "  },\n";
    
    json << "  \"memory\": {\n";
    json << "    \"total_size\": " << config.memory_config.total_memory_size << ",\n";
    json << "    \"page_size\": " << config.memory_config.page_size << ",\n";
    json << "    \"replacement_algorithm\": \"" << replacementAlgorithmToString(config.memory_config.replacement_algo) << "\"\n";
    json << "  },\n";
    
    json << "  \"simulation\": {\n";
    json << "    \"speed\": " << config.simulation_speed << ",\n";
    json << "    \"step_mode\": " << (config.step_mode ? "true" : "false") << "\n";
    json << "  },\n";
    
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
    // This is a simplified JSON parser for our specific format
    // In a production system, you'd use a proper JSON library like nlohmann/json
    
    config = getDefaultConfiguration();
    
    // Parse scheduling algorithm
    size_t algo_pos = json_string.find("\"algorithm\":");
    if (algo_pos != std::string::npos) {
        size_t start = json_string.find("\"", algo_pos + 12);
        size_t end = json_string.find("\"", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            std::string algo_str = json_string.substr(start + 1, end - start - 1);
            config.scheduling_algo = schedulingAlgorithmFromString(algo_str);
        }
    }
    
    // Parse time quantum
    size_t quantum_pos = json_string.find("\"time_quantum\":");
    if (quantum_pos != std::string::npos) {
        size_t start = quantum_pos + 15;
        size_t end = json_string.find_first_of(",\n}", start);
        if (end != std::string::npos) {
            std::string quantum_str = json_string.substr(start, end - start);
            config.time_quantum = std::stoi(quantum_str);
        }
    }
    
    // Parse memory configuration
    size_t total_size_pos = json_string.find("\"total_size\":");
    if (total_size_pos != std::string::npos) {
        size_t start = total_size_pos + 13;
        size_t end = json_string.find_first_of(",\n}", start);
        if (end != std::string::npos) {
            std::string size_str = json_string.substr(start, end - start);
            config.memory_config.total_memory_size = std::stoi(size_str);
        }
    }
    
    size_t page_size_pos = json_string.find("\"page_size\":");
    if (page_size_pos != std::string::npos) {
        size_t start = page_size_pos + 12;
        size_t end = json_string.find_first_of(",\n}", start);
        if (end != std::string::npos) {
            std::string size_str = json_string.substr(start, end - start);
            config.memory_config.page_size = std::stoi(size_str);
            config.memory_config.num_frames = config.memory_config.total_memory_size / config.memory_config.page_size;
        }
    }
    
    // Parse replacement algorithm
    size_t repl_pos = json_string.find("\"replacement_algorithm\":");
    if (repl_pos != std::string::npos) {
        size_t start = json_string.find("\"", repl_pos + 24);
        size_t end = json_string.find("\"", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            std::string repl_str = json_string.substr(start + 1, end - start - 1);
            config.memory_config.replacement_algo = replacementAlgorithmFromString(repl_str);
        }
    }
    
    // Parse simulation speed
    size_t speed_pos = json_string.find("\"speed\":");
    if (speed_pos != std::string::npos) {
        size_t start = speed_pos + 8;
        size_t end = json_string.find_first_of(",\n}", start);
        if (end != std::string::npos) {
            std::string speed_str = json_string.substr(start, end - start);
            config.simulation_speed = std::stoi(speed_str);
        }
    }
    
    // Parse step mode
    size_t step_pos = json_string.find("\"step_mode\":");
    if (step_pos != std::string::npos) {
        size_t start = step_pos + 12;
        size_t end = json_string.find_first_of(",\n}", start);
        if (end != std::string::npos) {
            std::string step_str = json_string.substr(start, end - start);
            config.step_mode = (step_str.find("true") != std::string::npos);
        }
    }
    
    // Parse processes (simplified - would need more robust parsing for production)
    config.processes.clear();
    size_t processes_start = json_string.find("\"processes\": [");
    if (processes_start != std::string::npos) {
        size_t processes_end = json_string.find("]", processes_start);
        if (processes_end != std::string::npos) {
            std::string processes_section = json_string.substr(processes_start, processes_end - processes_start);
            
            // Count process objects
            size_t pos = 0;
            while ((pos = processes_section.find("{", pos)) != std::string::npos) {
                size_t obj_end = processes_section.find("}", pos);
                if (obj_end != std::string::npos) {
                    std::string process_obj = processes_section.substr(pos, obj_end - pos + 1);
                    
                    ProcessConfig process;
                    
                    // Parse priority
                    size_t prio_pos = process_obj.find("\"priority\":");
                    if (prio_pos != std::string::npos) {
                        size_t start = prio_pos + 11;
                        size_t end = process_obj.find_first_of(",\n}", start);
                        if (end != std::string::npos) {
                            process.priority = std::stoi(process_obj.substr(start, end - start));
                        }
                    }
                    
                    // Parse burst time
                    size_t burst_pos = process_obj.find("\"burst_time\":");
                    if (burst_pos != std::string::npos) {
                        size_t start = burst_pos + 13;
                        size_t end = process_obj.find_first_of(",\n}", start);
                        if (end != std::string::npos) {
                            process.burst_time = std::stoi(process_obj.substr(start, end - start));
                        }
                    }
                    
                    // Parse arrival time
                    size_t arrival_pos = process_obj.find("\"arrival_time\":");
                    if (arrival_pos != std::string::npos) {
                        size_t start = arrival_pos + 15;
                        size_t end = process_obj.find_first_of(",\n}", start);
                        if (end != std::string::npos) {
                            process.arrival_time = std::stoi(process_obj.substr(start, end - start));
                        }
                    }
                    
                    // Parse memory requirement
                    size_t mem_pos = process_obj.find("\"memory_requirement\":");
                    if (mem_pos != std::string::npos) {
                        size_t start = mem_pos + 21;
                        size_t end = process_obj.find_first_of(",\n}", start);
                        if (end != std::string::npos) {
                            process.memory_requirement = std::stoi(process_obj.substr(start, end - start));
                        }
                    }
                    
                    config.processes.push_back(process);
                }
                pos = obj_end + 1;
            }
        }
    }
    
    return validateConfiguration(config).is_valid;
}

std::string ConfigurationManager::schedulingAlgorithmToString(SchedulingAlgorithm algorithm) {
    switch (algorithm) {
        case SchedulingAlgorithm::FCFS:
            return "FCFS";
        case SchedulingAlgorithm::ROUND_ROBIN:
            return "RoundRobin";
        case SchedulingAlgorithm::PRIORITY:
            return "Priority";
        default:
            return "FCFS";
    }
}

SchedulingAlgorithm ConfigurationManager::schedulingAlgorithmFromString(const std::string& str) {
    if (str == "RoundRobin") {
        return SchedulingAlgorithm::ROUND_ROBIN;
    } else if (str == "Priority") {
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

ReplacementAlgorithm ConfigurationManager::replacementAlgorithmFromString(const std::string& str) {
    if (str == "FIFO") {
        return ReplacementAlgorithm::FIFO;
    } else {
        return ReplacementAlgorithm::LRU;
    }
}

std::string ConfigurationManager::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

bool ConfigurationManager::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}