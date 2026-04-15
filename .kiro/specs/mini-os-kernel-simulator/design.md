# Design Document: Mini OS Kernel Simulator

## Overview

The Mini OS Kernel Simulator is a comprehensive educational software system designed to simulate core operating system kernel functionalities. Built with C++17 and Qt framework, it provides an interactive environment for students and educators to explore process management, CPU scheduling algorithms, memory management with paging, synchronization primitives, and performance analysis.

The system follows a Model-View-Controller (MVC) architecture pattern, ensuring clear separation of concerns between the simulation engine, user interface, and application logic. The simulator supports up to 100 concurrent processes while maintaining real-time performance with time step processing completed within 100ms.

### Key Features

- **Process Management**: Complete process lifecycle simulation with state management
- **CPU Scheduling**: Implementation of FCFS, Round Robin, and Priority scheduling algorithms
- **Memory Management**: Paging system with FIFO and LRU replacement algorithms
- **Synchronization**: Semaphores and mutexes for process coordination
- **Performance Metrics**: Real-time collection and analysis of system performance
- **Interactive GUI**: Comprehensive dashboard with visualization components
- **Configuration Management**: Save/load simulation scenarios
- **Export Capabilities**: Results export in multiple formats

## Architecture

### System Architecture Overview

The Mini OS Kernel Simulator employs a layered MVC architecture with the following primary components:

```
┌─────────────────────────────────────────────────────────────┐
│                    Presentation Layer (View)                │
├─────────────────────────────────────────────────────────────┤
│  Dashboard │ Process Table │ Gantt Chart │ Memory View │    │
│            │               │             │             │    │
│  Metrics Panel │ Configuration Dialog │ Export Dialog │    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                 Application Layer (Controller)              │
├─────────────────────────────────────────────────────────────┤
│  Simulation Controller │ GUI Controller │ Export Controller │
│                        │                │                   │
│  Configuration Manager │ Event Handler  │ Validation Layer  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   Business Logic Layer (Model)              │
├─────────────────────────────────────────────────────────────┤
│                        Kernel Core                          │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐           │
│  │   Process   │ │  Scheduler  │ │   Memory    │           │
│  │   Manager   │ │             │ │   Manager   │           │
│  └─────────────┘ └─────────────┘ └─────────────┘           │
│  ┌─────────────┐ ┌─────────────┐                           │
│  │    Sync     │ │   Metrics   │                           │
│  │   Manager   │ │  Collector  │                           │
│  └─────────────┘ └─────────────┘                           │
└─────────────────────────────────────────────────────────────┘
```

### Component Responsibilities

**Presentation Layer (Qt GUI Components)**
- Dashboard: Main control interface with simulation controls
- Process Table: Real-time process state visualization
- Gantt Chart: Timeline visualization of process execution
- Memory View: Visual representation of memory allocation
- Metrics Panel: Performance statistics display

**Application Layer (Controllers)**
- Simulation Controller: Orchestrates simulation execution and timing
- GUI Controller: Manages user interface interactions and updates
- Configuration Manager: Handles save/load operations
- Export Controller: Manages result export functionality

**Business Logic Layer (Kernel Core)**
- Process Manager: Process lifecycle and state management
- Scheduler: CPU scheduling algorithm implementations
- Memory Manager: Paging and memory allocation
- Sync Manager: Synchronization primitives
- Metrics Collector: Performance data collection and analysis

## Components and Interfaces

### Core Kernel Components

#### Process Manager
```cpp
class ProcessManager {
public:
    struct ProcessControlBlock {
        int pid;
        ProcessState state;
        int priority;
        int burst_time;
        int remaining_time;
        int arrival_time;
        int start_time;
        int completion_time;
        std::vector<int> allocated_pages;
    };
    
    int createProcess(int priority, int burst_time, int arrival_time);
    void terminateProcess(int pid);
    void updateProcessState(int pid, ProcessState new_state);
    ProcessControlBlock* getProcess(int pid);
    std::vector<ProcessControlBlock*> getAllProcesses();
    
private:
    std::unordered_map<int, std::unique_ptr<ProcessControlBlock>> processes;
    int next_pid;
    std::mutex process_mutex;
};
```

#### Scheduler Interface
```cpp
class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual int selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) = 0;
    virtual bool shouldPreempt(int current_pid, int new_pid) = 0;
    virtual void onTimeQuantumExpired() = 0;
    virtual void reset() = 0;
};

class FCFSScheduler : public Scheduler { /* Implementation */ };
class RoundRobinScheduler : public Scheduler { 
    int time_quantum;
    int current_quantum_remaining;
};
class PriorityScheduler : public Scheduler { /* Implementation */ };
```

#### Memory Manager
```cpp
class MemoryManager {
public:
    struct PageFrame {
        int frame_id;
        int process_id;
        int page_number;
        bool is_allocated;
        int last_access_time;
        int allocation_time;
    };
    
    struct PageTableEntry {
        int virtual_page;
        int physical_frame;
        bool is_valid;
        bool is_dirty;
    };
    
    bool allocatePages(int process_id, int num_pages);
    void deallocatePages(int process_id);
    int handlePageFault(int process_id, int virtual_page);
    void setReplacementAlgorithm(ReplacementAlgorithm algorithm);
    
private:
    std::vector<PageFrame> page_frames;
    std::unordered_map<int, std::vector<PageTableEntry>> page_tables;
    std::unique_ptr<PageReplacementAlgorithm> replacement_algorithm;
    int total_frames;
    int frame_size;
};
```

#### Synchronization Manager
```cpp
class SyncManager {
public:
    class Semaphore {
        int count;
        std::queue<int> waiting_processes;
        std::mutex sem_mutex;
    public:
        void wait(int process_id);
        void signal();
        int getCount() const;
    };
    
    class Mutex {
        int owner_process;
        std::queue<int> waiting_processes;
        std::mutex mtx_mutex;
    public:
        bool lock(int process_id);
        void unlock(int process_id);
        bool isLocked() const;
    };
    
    int createSemaphore(int initial_count);
    int createMutex();
    Semaphore* getSemaphore(int sem_id);
    Mutex* getMutex(int mutex_id);
    
private:
    std::unordered_map<int, std::unique_ptr<Semaphore>> semaphores;
    std::unordered_map<int, std::unique_ptr<Mutex>> mutexes;
    int next_sem_id;
    int next_mutex_id;
};
```

### GUI Component Interfaces

#### Main Dashboard
```cpp
class Dashboard : public QWidget {
    Q_OBJECT
    
public:
    Dashboard(QWidget* parent = nullptr);
    
public slots:
    void onStartSimulation();
    void onPauseSimulation();
    void onStopSimulation();
    void onResetSimulation();
    void onStepSimulation();
    void onSpeedChanged(int speed);
    
signals:
    void simulationStartRequested();
    void simulationPaused();
    void simulationStopped();
    void simulationReset();
    void stepRequested();
    void speedChanged(int steps_per_second);
    
private:
    QPushButton* start_button;
    QPushButton* pause_button;
    QPushButton* stop_button;
    QPushButton* reset_button;
    QPushButton* step_button;
    QSlider* speed_slider;
    QLabel* time_display;
    QComboBox* scheduler_combo;
    QComboBox* memory_algo_combo;
};
```

#### Process Table Widget
```cpp
class ProcessTableWidget : public QTableWidget {
    Q_OBJECT
    
public:
    ProcessTableWidget(QWidget* parent = nullptr);
    void updateProcesses(const std::vector<ProcessControlBlock*>& processes);
    void highlightRunningProcess(int pid);
    
private:
    void setupColumns();
    void addProcessRow(const ProcessControlBlock* process);
    void updateProcessRow(int row, const ProcessControlBlock* process);
    
    enum Columns {
        PID = 0,
        STATE = 1,
        PRIORITY = 2,
        BURST_TIME = 3,
        REMAINING_TIME = 4,
        ARRIVAL_TIME = 5
    };
};
```

## Data Models

### Process State Model
```cpp
enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

struct ProcessMetrics {
    int waiting_time;
    int turnaround_time;
    int response_time;
    int completion_time;
};
```

### Memory Model
```cpp
struct MemoryConfiguration {
    int total_memory_size;
    int page_size;
    int num_frames;
    ReplacementAlgorithm replacement_algo;
};

enum class ReplacementAlgorithm {
    FIFO,
    LRU
};
```

### Simulation Configuration Model
```cpp
struct SimulationConfig {
    struct ProcessConfig {
        int priority;
        int burst_time;
        int arrival_time;
        int memory_requirement;
    };
    
    std::vector<ProcessConfig> processes;
    SchedulingAlgorithm scheduling_algo;
    int time_quantum;  // For Round Robin
    MemoryConfiguration memory_config;
    int simulation_speed;  // Steps per second
    bool step_mode;
};
```

### Performance Metrics Model
```cpp
struct SystemMetrics {
    double average_waiting_time;
    double average_turnaround_time;
    double average_response_time;
    double cpu_utilization;
    double throughput;
    int total_processes;
    int completed_processes;
    int current_time;
    
    // Memory metrics
    double memory_utilization;
    int page_faults;
    int page_replacements;
};
```

## Algorithms

### CPU Scheduling Algorithms

#### First-Come-First-Served (FCFS)
```cpp
int FCFSScheduler::selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) {
    if (ready_queue.empty()) return -1;
    
    // Find process with earliest arrival time
    auto earliest = std::min_element(ready_queue.begin(), ready_queue.end(),
        [](const ProcessControlBlock* a, const ProcessControlBlock* b) {
            return a->arrival_time < b->arrival_time;
        });
    
    return (*earliest)->pid;
}
```

#### Round Robin
```cpp
int RoundRobinScheduler::selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) {
    if (ready_queue.empty()) return -1;
    
    // Circular queue implementation
    static int last_scheduled_index = -1;
    int next_index = (last_scheduled_index + 1) % ready_queue.size();
    last_scheduled_index = next_index;
    
    current_quantum_remaining = time_quantum;
    return ready_queue[next_index]->pid;
}

void RoundRobinScheduler::onTimeQuantumExpired() {
    current_quantum_remaining = 0;
    // Signal preemption needed
}
```

#### Priority Scheduling
```cpp
int PriorityScheduler::selectNextProcess(const std::vector<ProcessControlBlock*>& ready_queue) {
    if (ready_queue.empty()) return -1;
    
    // Find highest priority process (lower number = higher priority)
    auto highest_priority = std::min_element(ready_queue.begin(), ready_queue.end(),
        [](const ProcessControlBlock* a, const ProcessControlBlock* b) {
            return a->priority < b->priority;
        });
    
    return (*highest_priority)->pid;
}

bool PriorityScheduler::shouldPreempt(int current_pid, int new_pid) {
    auto current_process = process_manager->getProcess(current_pid);
    auto new_process = process_manager->getProcess(new_pid);
    
    return new_process->priority < current_process->priority;
}
```

### Memory Management Algorithms

#### FIFO Page Replacement
```cpp
class FIFOReplacementAlgorithm : public PageReplacementAlgorithm {
public:
    int selectVictimFrame(const std::vector<PageFrame>& frames) override {
        int oldest_time = INT_MAX;
        int victim_frame = -1;
        
        for (int i = 0; i < frames.size(); ++i) {
            if (frames[i].is_allocated && frames[i].allocation_time < oldest_time) {
                oldest_time = frames[i].allocation_time;
                victim_frame = i;
            }
        }
        
        return victim_frame;
    }
};
```

#### LRU Page Replacement
```cpp
class LRUReplacementAlgorithm : public PageReplacementAlgorithm {
public:
    int selectVictimFrame(const std::vector<PageFrame>& frames) override {
        int least_recent_time = INT_MAX;
        int victim_frame = -1;
        
        for (int i = 0; i < frames.size(); ++i) {
            if (frames[i].is_allocated && frames[i].last_access_time < least_recent_time) {
                least_recent_time = frames[i].last_access_time;
                victim_frame = i;
            }
        }
        
        return victim_frame;
    }
    
    void onPageAccess(int frame_id, int current_time) override {
        // Update last access time for LRU tracking
        if (frame_id >= 0 && frame_id < frames.size()) {
            frames[frame_id].last_access_time = current_time;
        }
    }
};
```

### Performance Metrics Calculation

#### Metrics Collection Algorithm
```cpp
void MetricsCollector::calculateMetrics() {
    double total_waiting_time = 0;
    double total_turnaround_time = 0;
    double total_response_time = 0;
    int completed_count = 0;
    
    for (const auto& [pid, process] : completed_processes) {
        int waiting_time = process.completion_time - process.arrival_time - process.burst_time;
        int turnaround_time = process.completion_time - process.arrival_time;
        int response_time = process.start_time - process.arrival_time;
        
        total_waiting_time += waiting_time;
        total_turnaround_time += turnaround_time;
        total_response_time += response_time;
        completed_count++;
    }
    
    if (completed_count > 0) {
        metrics.average_waiting_time = total_waiting_time / completed_count;
        metrics.average_turnaround_time = total_turnaround_time / completed_count;
        metrics.average_response_time = total_response_time / completed_count;
    }
    
    // CPU Utilization = (Total execution time) / (Total simulation time)
    metrics.cpu_utilization = (double)total_execution_time / current_simulation_time * 100.0;
    
    // Throughput = Completed processes / Total time
    metrics.throughput = (double)completed_count / current_simulation_time;
}
```

## GUI Specifications

### Main Window Layout
```
┌─────────────────────────────────────────────────────────────────────┐
│ File  Edit  Simulation  View  Help                                   │
├─────────────────────────────────────────────────────────────────────┤
│ ┌─────────────────┐ ┌─────────────────────────────────────────────┐ │
│ │   Dashboard     │ │            Process Table                    │ │
│ │                 │ │                                             │ │
│ │ [Start] [Pause] │ │ PID | State | Priority | Burst | Remaining  │ │
│ │ [Stop] [Reset]  │ │  1  | Ready |    3     |  10   |     8      │ │
│ │ [Step]          │ │  2  | Run   |    1     |   5   |     3      │ │
│ │                 │ │  3  | Wait  |    2     |   8   |     8      │ │
│ │ Speed: [====|]  │ │                                             │ │
│ │ Time: 00:15     │ │                                             │ │
│ │                 │ │                                             │ │
│ │ Scheduler:      │ │                                             │ │
│ │ [Round Robin ▼] │ │                                             │ │
│ │                 │ │                                             │ │
│ │ Memory Algo:    │ │                                             │ │
│ │ [LRU        ▼]  │ │                                             │ │
│ └─────────────────┘ └─────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────────────────┐ │
│ │                        Gantt Chart                              │ │
│ │ P1 ████████                                                     │ │
│ │ P2      ██████████                                              │ │
│ │ P3              ████████████                                    │ │
│ │    0    5    10    15    20    25    30                        │ │
│ └─────────────────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────────────┤
│ ┌─────────────────┐ ┌─────────────────────────────────────────────┐ │
│ │  Memory View    │ │           Metrics Panel                     │ │
│ │                 │ │                                             │ │
│ │ Frame 0: [P1]   │ │ Avg Waiting Time:    5.67 ms               │ │
│ │ Frame 1: [P2]   │ │ Avg Turnaround Time: 12.33 ms              │ │
│ │ Frame 2: [Free] │ │ CPU Utilization:     85.4%                 │ │
│ │ Frame 3: [P1]   │ │ Throughput:          2.1 proc/sec          │ │
│ │ Frame 4: [Free] │ │ Memory Utilization:  60.0%                 │ │
│ │ Frame 5: [P3]   │ │ Page Faults:         15                    │ │
│ │                 │ │ Page Replacements:   8                     │ │
│ │ Page Table P1:  │ │                                             │ │
│ │ VP0 -> PF0      │ │                                             │ │
│ │ VP1 -> PF3      │ │                                             │ │
│ └─────────────────┘ └─────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────┘
```

### Widget Specifications

#### Dashboard Controls
- **Start Button**: Initiates simulation execution
- **Pause Button**: Temporarily halts simulation (can be resumed)
- **Stop Button**: Terminates simulation completely
- **Reset Button**: Clears all data and returns to initial state
- **Step Button**: Advances simulation by one time step (debug mode)
- **Speed Slider**: Controls simulation speed (1-10 steps per second)
- **Time Display**: Shows current simulation time
- **Algorithm Selectors**: Dropdown menus for scheduling and memory algorithms

#### Process Table Columns
1. **PID**: Process identifier (integer)
2. **State**: Current process state (New/Ready/Running/Waiting/Terminated)
3. **Priority**: Process priority (1-10, lower = higher priority)
4. **Burst Time**: Total execution time required
5. **Remaining Time**: Time left for completion
6. **Arrival Time**: When process entered the system

#### Gantt Chart Features
- **Color Coding**: Each process has a unique color
- **Time Scale**: Horizontal axis showing time units
- **Process Blocks**: Rectangular blocks showing execution periods
- **Process Labels**: PID displayed within each block
- **Scrolling**: Horizontal scroll for long simulations
- **Zoom**: Ability to zoom in/out on timeline

#### Memory View Components
- **Frame Grid**: Visual grid showing all page frames
- **Allocation Status**: Color coding for allocated/free frames
- **Process Ownership**: Display which process owns each frame
- **Page Table**: Mapping of virtual pages to physical frames
- **Replacement Highlighting**: Visual indication of page replacements

#### Metrics Panel Display
- **Waiting Time**: Average time processes spend in ready queue
- **Turnaround Time**: Average total time from arrival to completion
- **Response Time**: Average time from arrival to first execution
- **CPU Utilization**: Percentage of time CPU is busy
- **Throughput**: Number of processes completed per time unit
- **Memory Metrics**: Memory utilization and paging statistics

## Error Handling

### Input Validation
```cpp
class InputValidator {
public:
    static ValidationResult validateProcessParameters(int priority, int burst_time, int arrival_time) {
        ValidationResult result;
        
        if (priority < 1 || priority > 10) {
            result.is_valid = false;
            result.error_message = "Priority must be between 1 and 10";
            return result;
        }
        
        if (burst_time <= 0) {
            result.is_valid = false;
            result.error_message = "Burst time must be positive";
            return result;
        }
        
        if (arrival_time < 0) {
            result.is_valid = false;
            result.error_message = "Arrival time cannot be negative";
            return result;
        }
        
        result.is_valid = true;
        return result;
    }
};
```

### Exception Handling Strategy
- **Resource Exhaustion**: Graceful handling when system limits are reached
- **Invalid Operations**: Prevention of illegal state transitions
- **File I/O Errors**: Comprehensive error reporting for configuration and export operations
- **Memory Allocation Failures**: Safe fallback mechanisms
- **GUI Exceptions**: Error dialogs with user-friendly messages

### Error Recovery Mechanisms
- **Automatic State Restoration**: Return to last known good state on critical errors
- **Partial Simulation Recovery**: Continue simulation with reduced functionality if possible
- **User Notification**: Clear error messages with suggested corrective actions
- **Logging**: Comprehensive error logging for debugging purposes

## Testing Strategy

The Mini OS Kernel Simulator requires comprehensive testing to ensure correctness of complex scheduling and memory management algorithms. The testing strategy combines unit tests for individual components with integration tests for system-wide behavior.

### Unit Testing Approach

**Component-Level Testing**:
- **Process Manager**: Test process creation, state transitions, and lifecycle management
- **Schedulers**: Verify correct process selection for each algorithm (FCFS, Round Robin, Priority)
- **Memory Manager**: Test page allocation, deallocation, and replacement algorithms
- **Synchronization**: Verify semaphore and mutex behavior under various conditions
- **Metrics Collector**: Test calculation accuracy for performance metrics

**GUI Component Testing**:
- **Widget Functionality**: Test user interactions and state updates
- **Data Display**: Verify correct visualization of simulation data
- **Input Validation**: Test parameter validation and error handling
- **Export Functions**: Test file generation and format correctness

### Integration Testing

**System Integration**:
- **End-to-End Simulation**: Complete simulation runs with various configurations
- **Algorithm Interaction**: Test interactions between scheduling and memory management
- **Performance Validation**: Verify system meets 100ms time step requirement
- **Concurrent Process Handling**: Test with up to 100 simultaneous processes

**GUI Integration**:
- **Real-time Updates**: Test GUI responsiveness during simulation
- **Configuration Management**: Test save/load functionality
- **Export Integration**: Test complete export workflow

### Performance Testing

**Timing Requirements**:
- Time step processing must complete within 100ms
- GUI updates must not block simulation execution
- Memory operations must scale efficiently with process count

**Load Testing**:
- Test with maximum 100 concurrent processes
- Verify memory usage remains within acceptable bounds
- Test long-running simulations for memory leaks

**Stress Testing**:
- Test with rapid process creation/termination
- Test with maximum memory allocation scenarios
- Test GUI responsiveness under high load

This comprehensive testing approach ensures the simulator meets all functional requirements while maintaining performance and reliability standards for educational use.

## Correctness Properties

*A property is a characteristic or behavior that should hold true across all valid executions of a system-essentially, a formal statement about what the system should do. Properties serve as the bridge between human-readable specifications and machine-verifiable correctness guarantees.*

### Property 1: Process Creation and Management

*For any* valid process parameters (priority, burst time, arrival time), creating a process SHALL result in a unique PID assignment, correct initial state (NEW), and proper initialization of all process control block fields with the provided values.

**Validates: Requirements 1.1, 1.2, 1.6**

### Property 2: Process State Transitions

*For any* process and any valid state transition sequence, the Process Manager SHALL update process states immediately and maintain state consistency throughout the process lifecycle.

**Validates: Requirements 1.3, 1.4**

### Property 3: Process Termination and Cleanup

*For any* process with allocated resources, termination SHALL completely clean up all associated resources including memory pages, synchronization objects, and process control blocks.

**Validates: Requirements 1.5**

### Property 4: FCFS Scheduling Correctness

*For any* set of processes with different arrival times, the FCFS scheduler SHALL always select the process with the earliest arrival time from the ready queue.

**Validates: Requirements 2.1, 2.4**

### Property 5: Round Robin Scheduling Correctness

*For any* set of processes and any valid time quantum, the Round Robin scheduler SHALL cycle through processes in order and preempt processes exactly when the time quantum expires.

**Validates: Requirements 2.2, 2.4, 2.5**

### Property 6: Priority Scheduling Correctness

*For any* set of processes with different priorities, the Priority scheduler SHALL always select the highest priority process and preempt lower priority processes when higher priority processes arrive.

**Validates: Requirements 2.3, 2.4, 2.6**

### Property 7: Scheduler Queue Management

*For any* scheduling algorithm and any sequence of process operations, the scheduler SHALL maintain separate, consistent ready queues for each algorithm.

**Validates: Requirements 2.7**

### Property 8: Memory Allocation and Paging

*For any* memory allocation request when free frames are available, the Memory Manager SHALL allocate fixed-size page frames and maintain correct virtual-to-physical address mappings in the page table.

**Validates: Requirements 3.1, 3.2, 3.3**

### Property 9: Page Replacement Algorithm Correctness

*For any* memory allocation request when no free frames are available, the Memory Manager SHALL apply the selected page replacement algorithm (FIFO or LRU) correctly and update page tables accordingly.

**Validates: Requirements 3.4, 3.5, 3.6**

### Property 10: Page Fault Handling

*For any* page fault scenario, the Memory Manager SHALL handle the fault correctly, allocate or replace pages as needed, and update page tables to reflect the new mappings.

**Validates: Requirements 3.7**

### Property 11: Memory Usage Tracking

*For any* sequence of memory allocation and deallocation operations, the Memory Manager SHALL accurately track page frame allocation status and usage statistics.

**Validates: Requirements 3.8**

### Property 12: Semaphore Synchronization

*For any* semaphore with any initial count and any sequence of wait/signal operations, the Sync Manager SHALL correctly decrement/increment the counter, block processes when counter reaches zero, and wake waiting processes when signaled.

**Validates: Requirements 4.1, 4.3, 4.4, 4.5**

### Property 13: Mutex Synchronization

*For any* mutex and any sequence of lock/unlock operations, the Sync Manager SHALL ensure mutual exclusion by allowing only one process to hold the lock at any time.

**Validates: Requirements 4.2, 4.6**

### Property 14: Deadlock Prevention

*For any* resource request pattern that could potentially cause deadlock, the Sync Manager SHALL prevent deadlock through proper resource ordering and detection mechanisms.

**Validates: Requirements 4.7**

### Property 15: Performance Metrics Accuracy

*For any* simulation execution with any set of processes, the Metrics Collector SHALL calculate mathematically correct average waiting time, turnaround time, response time, CPU utilization, and throughput based on actual process execution data.

**Validates: Requirements 5.1, 5.2, 5.3, 5.4, 5.5**

### Property 16: Real-time Metrics Updates

*For any* simulation execution, the Metrics Collector SHALL update all performance metrics in real-time during each simulation time step and generate comprehensive reports upon completion.

**Validates: Requirements 5.6, 5.7**

### Property 17: GUI Real-time Updates

*For any* simulation state change (process state, memory allocation, scheduling decision), all relevant GUI components (Process Table, Gantt Chart, Memory View, Metrics Panel) SHALL update immediately to reflect the current system state.

**Validates: Requirements 6.5, 7.3, 7.6, 8.6, 9.6, 10.6**

### Property 18: Input Validation

*For any* user input parameters (process parameters, configuration values), the system SHALL validate inputs before processing and display specific error messages for invalid inputs.

**Validates: Requirements 6.7, 15.1, 15.2**

### Property 19: Process Table Display Accuracy

*For any* set of active processes, the Process Table SHALL display all processes with correct information in all required columns, highlight the running process, and maintain PID-based ascending sort order.

**Validates: Requirements 7.1, 7.4, 7.5**

### Property 20: Gantt Chart Visualization

*For any* process execution sequence, the Gantt Chart SHALL display the execution timeline with unique colors for each process, correct time blocks, process IDs within blocks, and support horizontal scrolling for long simulations.

**Validates: Requirements 8.1, 8.2, 8.3, 8.5, 8.7**

### Property 21: Memory View Accuracy

*For any* memory allocation state, the Memory View SHALL correctly display frame allocation status, process ownership, page replacement highlighting, and page table mappings for selected processes.

**Validates: Requirements 9.1, 9.2, 9.3, 9.4, 9.5**

### Property 22: Metrics Display Formatting

*For any* performance metric values, the Metrics Panel SHALL display waiting time and turnaround time with exactly two decimal precision, CPU utilization as percentage, and throughput in correct format.

**Validates: Requirements 10.2, 10.3, 10.4, 10.5**

### Property 23: Metrics Reset Behavior

*For any* simulation restart operation, the Metrics Panel SHALL reset all displayed metrics to initial values.

**Validates: Requirements 10.7**

### Property 24: Simulation Time Management

*For any* simulation execution mode (continuous or step-by-step), the Simulator SHALL execute in discrete time steps, support configurable speed, advance exactly one step per command in step mode, and display current time prominently.

**Validates: Requirements 11.1, 11.2, 11.3, 11.4, 11.5**

### Property 25: Simulation Control Operations

*For any* simulation state, the Simulator SHALL correctly handle pause/resume operations, maintaining simulation state consistency across control operations.

**Validates: Requirements 11.6**

### Property 26: Configuration Management Round-trip

*For any* valid simulation configuration, saving the configuration to file and then loading it SHALL result in identical configuration parameters and algorithm selections.

**Validates: Requirements 12.1, 12.2, 12.3**

### Property 27: Configuration File Format

*For any* saved configuration, the configuration file SHALL use human-readable format (JSON or XML) and include all necessary parameters for complete simulation restoration.

**Validates: Requirements 12.4**

### Property 28: Configuration Validation

*For any* configuration loading attempt, the system SHALL validate all parameters before applying them and display descriptive error messages for invalid configurations.

**Validates: Requirements 12.5, 12.6**

### Property 29: Export Functionality

*For any* simulation state, the system SHALL successfully export Gantt charts as image files, performance metrics as CSV files, and simulation logs as text files, including timestamp and configuration information in all exports.

**Validates: Requirements 13.1, 13.2, 13.3, 13.4**

### Property 30: Export Error Handling

*For any* export operation failure, the system SHALL display clear error messages indicating the cause of failure.

**Validates: Requirements 13.6**

### Property 31: Performance Requirements

*For any* simulation with up to 100 concurrent processes, the system SHALL complete time step processing within 100 milliseconds while maintaining system stability.

**Validates: Requirements 14.3, 14.4**

### Property 32: Error Handling and System Stability

*For any* error condition (resource exhaustion, memory allocation failure, file operation failure), the system SHALL handle errors gracefully, maintain system stability, provide clear error descriptions, and log error events appropriately.

**Validates: Requirements 14.7, 15.3, 15.4, 15.5, 15.6, 15.7**

## Error Handling

### Input Validation
```cpp
class InputValidator {
public:
    static ValidationResult validateProcessParameters(int priority, int burst_time, int arrival_time) {
        ValidationResult result;
        
        if (priority < 1 || priority > 10) {
            result.is_valid = false;
            result.error_message = "Priority must be between 1 and 10";
            return result;
        }
        
        if (burst_time <= 0) {
            result.is_valid = false;
            result.error_message = "Burst time must be positive";
            return result;
        }
        
        if (arrival_time < 0) {
            result.is_valid = false;
            result.error_message = "Arrival time cannot be negative";
            return result;
        }
        
        result.is_valid = true;
        return result;
    }
};
```

### Exception Handling Strategy
- **Resource Exhaustion**: Graceful handling when system limits are reached
- **Invalid Operations**: Prevention of illegal state transitions
- **File I/O Errors**: Comprehensive error reporting for configuration and export operations
- **Memory Allocation Failures**: Safe fallback mechanisms
- **GUI Exceptions**: Error dialogs with user-friendly messages

### Error Recovery Mechanisms
- **Automatic State Restoration**: Return to last known good state on critical errors
- **Partial Simulation Recovery**: Continue simulation with reduced functionality if possible
- **User Notification**: Clear error messages with suggested corrective actions
- **Logging**: Comprehensive error logging for debugging purposes

## Testing Strategy

The Mini OS Kernel Simulator requires comprehensive testing to ensure correctness of complex scheduling and memory management algorithms. The testing strategy combines unit tests for individual components with property-based tests for universal behaviors and integration tests for system-wide functionality.

### Property-Based Testing Approach

**Property Test Configuration**:
- Minimum 100 iterations per property test to ensure comprehensive input coverage
- Each property test references its corresponding design document property
- Tag format: **Feature: mini-os-kernel-simulator, Property {number}: {property_text}**
- Use fast-check library for JavaScript/TypeScript or Hypothesis for Python, or QuickCheck-style library for C++

**Core Property Categories**:
- **Process Management Properties**: Test process lifecycle, state transitions, and resource management
- **Scheduling Algorithm Properties**: Verify correctness of FCFS, Round Robin, and Priority scheduling
- **Memory Management Properties**: Test paging, page replacement algorithms, and memory allocation
- **Synchronization Properties**: Verify semaphore and mutex behavior under various conditions
- **Performance Metrics Properties**: Test calculation accuracy and real-time updates
- **GUI Behavior Properties**: Test real-time updates and display accuracy

### Unit Testing Approach

**Component-Level Testing**:
- **Process Manager**: Test specific process creation scenarios and edge cases
- **Schedulers**: Test algorithm-specific behaviors with concrete examples
- **Memory Manager**: Test specific allocation patterns and replacement scenarios
- **Synchronization**: Test specific semaphore/mutex usage patterns
- **Metrics Collector**: Test calculation edge cases (division by zero prevention)

**GUI Component Testing**:
- **Widget Functionality**: Test specific user interactions and state updates
- **Data Display**: Test display formatting and validation with concrete examples
- **Input Validation**: Test specific invalid input scenarios
- **Export Functions**: Test file generation with specific configurations

### Integration Testing

**System Integration**:
- **End-to-End Simulation**: Complete simulation runs with predefined configurations
- **Algorithm Interaction**: Test interactions between scheduling and memory management
- **Performance Validation**: Verify system meets 100ms time step requirement with various loads
- **Concurrent Process Handling**: Test with specific process counts approaching the 100-process limit

**GUI Integration**:
- **Real-time Updates**: Test GUI responsiveness during simulation with specific scenarios
- **Configuration Management**: Test save/load functionality with specific configurations
- **Export Integration**: Test complete export workflow with known simulation states

### Performance Testing

**Timing Requirements**:
- Time step processing must complete within 100ms across various simulation loads
- GUI updates must not block simulation execution
- Memory operations must scale efficiently with process count

**Load Testing**:
- Test with exactly 100 concurrent processes (maximum capacity)
- Verify memory usage remains within acceptable bounds during long simulations
- Test extended simulation runs for memory leaks

**Stress Testing**:
- Test with rapid process creation/termination cycles
- Test with maximum memory allocation scenarios
- Test GUI responsiveness under high computational load

### Testing Technology Stack

**Property-Based Testing Library**: 
- For C++: Use Catch2 with custom property generators or RapidCheck
- Minimum 100 iterations per property test
- Custom generators for process parameters, memory states, and scheduling scenarios

**Unit Testing Framework**:
- Catch2 or Google Test for C++ unit tests
- Qt Test framework for GUI component testing
- Mock objects for isolating component dependencies

**Integration Testing**:
- Custom test harness for end-to-end simulation testing
- Performance measurement tools for timing validation
- Memory profiling tools for leak detection

This comprehensive testing approach ensures the simulator meets all functional requirements while maintaining performance and reliability standards for educational use. The combination of property-based testing for universal behaviors, unit testing for specific scenarios, and integration testing for system-wide functionality provides thorough coverage of the complex operating system simulation logic.