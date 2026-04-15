# Requirements Document

## Introduction

The Mini OS Kernel Simulator is a C++ educational software system that simulates core operating system kernel functionalities including process management, CPU scheduling, memory management, synchronization primitives, and performance metrics collection. The system provides a graphical user interface for interactive simulation and visualization of operating system concepts.

## Glossary

- **Simulator**: The complete Mini OS Kernel Simulator application
- **GUI**: The graphical user interface component built with Qt framework
- **Controller**: The application layer that coordinates between GUI and Kernel Core
- **Kernel_Core**: The core simulation engine containing all OS subsystems
- **Process_Manager**: Component responsible for process lifecycle simulation
- **Scheduler**: Component implementing CPU scheduling algorithms
- **Memory_Manager**: Component managing simulated memory with paging
- **Sync_Manager**: Component handling synchronization primitives
- **Metrics_Collector**: Component gathering and calculating performance statistics
- **Process**: A simulated process with PID, state, and execution characteristics
- **Time_Step**: A discrete simulation time unit for the simulation loop
- **Gantt_Chart**: Visual timeline representation of process execution
- **Page_Frame**: A fixed-size memory block in the paging system
- **Page_Table**: Data structure mapping virtual pages to physical frames

## Requirements

### Requirement 1: Process Management System

**User Story:** As an educator, I want to simulate process lifecycle management, so that students can understand how operating systems handle process creation, execution, and termination.

#### Acceptance Criteria

1. THE Process_Manager SHALL create processes with unique Process Identifiers (PIDs)
2. WHEN a process is created, THE Process_Manager SHALL assign it initial state, priority, burst time, and arrival time
3. THE Process_Manager SHALL maintain process states including New, Ready, Running, Waiting, and Terminated
4. WHEN a process state changes, THE Process_Manager SHALL update the process state immediately
5. THE Process_Manager SHALL support process termination and cleanup of associated resources
6. THE Process_Manager SHALL maintain a process control block for each active process

### Requirement 2: CPU Scheduling Algorithms

**User Story:** As a student, I want to observe different CPU scheduling algorithms in action, so that I can compare their behavior and performance characteristics.

#### Acceptance Criteria

1. THE Scheduler SHALL implement First-Come-First-Served (FCFS) scheduling algorithm
2. THE Scheduler SHALL implement Round Robin scheduling with configurable time quantum
3. THE Scheduler SHALL implement Priority-based scheduling with preemption support
4. WHEN a scheduling decision is needed, THE Scheduler SHALL select the next process according to the active algorithm
5. WHILE Round Robin is active, THE Scheduler SHALL preempt processes after time quantum expiration
6. WHEN a higher priority process arrives, THE Scheduler SHALL preempt lower priority running processes
7. THE Scheduler SHALL maintain separate ready queues for each scheduling algorithm

### Requirement 3: Memory Management with Paging

**User Story:** As an instructor, I want to demonstrate memory management concepts, so that students can visualize how virtual memory and paging work in operating systems.

#### Acceptance Criteria

1. THE Memory_Manager SHALL implement a paging system with fixed-size page frames
2. THE Memory_Manager SHALL maintain a page table for virtual-to-physical address translation
3. WHEN a process requests memory, THE Memory_Manager SHALL allocate available page frames
4. IF no free frames are available, THEN THE Memory_Manager SHALL apply page replacement algorithm
5. THE Memory_Manager SHALL implement First-In-First-Out (FIFO) page replacement algorithm
6. THE Memory_Manager SHALL implement Least Recently Used (LRU) page replacement algorithm
7. WHEN a page fault occurs, THE Memory_Manager SHALL handle the fault and update page tables
8. THE Memory_Manager SHALL track page frame allocation status and usage statistics

### Requirement 4: Synchronization Primitives

**User Story:** As a student, I want to experiment with synchronization mechanisms, so that I can understand how processes coordinate access to shared resources.

#### Acceptance Criteria

1. THE Sync_Manager SHALL implement semaphore synchronization primitives
2. THE Sync_Manager SHALL implement mutex synchronization primitives
3. WHEN a process requests a semaphore, THE Sync_Manager SHALL decrement the semaphore counter
4. IF a semaphore counter reaches zero, THEN THE Sync_Manager SHALL block requesting processes
5. WHEN a semaphore is released, THE Sync_Manager SHALL increment counter and wake waiting processes
6. THE Sync_Manager SHALL ensure mutual exclusion for mutex-protected critical sections
7. THE Sync_Manager SHALL prevent deadlock situations through proper resource ordering

### Requirement 5: Performance Metrics Collection

**User Story:** As an educator, I want to collect and display performance metrics, so that students can analyze the efficiency of different scheduling and memory management algorithms.

#### Acceptance Criteria

1. THE Metrics_Collector SHALL calculate average waiting time for all processes
2. THE Metrics_Collector SHALL calculate average turnaround time for all processes
3. THE Metrics_Collector SHALL track CPU utilization percentage over simulation time
4. THE Metrics_Collector SHALL record throughput as processes completed per time unit
5. THE Metrics_Collector SHALL calculate response time for interactive processes
6. WHEN simulation completes, THE Metrics_Collector SHALL generate comprehensive performance report
7. THE Metrics_Collector SHALL update metrics in real-time during simulation execution

### Requirement 6: Graphical User Interface Dashboard

**User Story:** As a user, I want an intuitive graphical interface, so that I can easily control the simulation and input parameters.

#### Acceptance Criteria

1. THE GUI SHALL provide a main dashboard with simulation control buttons
2. THE GUI SHALL include input fields for process parameters (burst time, priority, arrival time)
3. THE GUI SHALL provide dropdown menus for selecting scheduling algorithms
4. THE GUI SHALL include controls for memory management algorithm selection
5. WHEN simulation is running, THE GUI SHALL display real-time status indicators
6. THE GUI SHALL provide start, pause, stop, and reset simulation controls
7. THE GUI SHALL validate user input parameters before simulation execution

### Requirement 7: Process Visualization Table

**User Story:** As a student, I want to see current process information in a table format, so that I can monitor process states and characteristics during simulation.

#### Acceptance Criteria

1. THE GUI SHALL display a process table showing all active processes
2. THE Process_Table SHALL include columns for PID, State, Priority, Burst Time, and Remaining Time
3. WHEN process state changes, THE Process_Table SHALL update the display immediately
4. THE Process_Table SHALL highlight the currently running process
5. THE Process_Table SHALL sort processes by PID in ascending order
6. THE Process_Table SHALL refresh automatically during simulation execution

### Requirement 8: Gantt Chart Visualization

**User Story:** As an instructor, I want to display a Gantt chart of process execution, so that students can visualize the timeline and scheduling decisions.

#### Acceptance Criteria

1. THE GUI SHALL render a Gantt chart showing process execution timeline
2. THE Gantt_Chart SHALL use different colors for each process
3. WHEN a process executes, THE Gantt_Chart SHALL add a time block for that process
4. THE Gantt_Chart SHALL display time units on the horizontal axis
5. THE Gantt_Chart SHALL show process IDs within execution blocks
6. THE Gantt_Chart SHALL update in real-time as simulation progresses
7. THE Gantt_Chart SHALL support horizontal scrolling for long simulations

### Requirement 9: Memory Visualization Interface

**User Story:** As a student, I want to see memory frame allocation visually, so that I can understand how paging and page replacement work.

#### Acceptance Criteria

1. THE GUI SHALL display a memory view showing page frame status
2. THE Memory_View SHALL indicate which frames are allocated and which are free
3. THE Memory_View SHALL show which process owns each allocated frame
4. WHEN page replacement occurs, THE Memory_View SHALL highlight the replaced frame
5. THE Memory_View SHALL display the page table for the currently selected process
6. THE Memory_View SHALL update immediately when memory allocation changes

### Requirement 10: Metrics Display Panel

**User Story:** As an educator, I want to display performance metrics in real-time, so that students can observe how different algorithms affect system performance.

#### Acceptance Criteria

1. THE GUI SHALL provide a metrics panel displaying current performance statistics
2. THE Metrics_Panel SHALL show average waiting time with two decimal precision
3. THE Metrics_Panel SHALL show average turnaround time with two decimal precision
4. THE Metrics_Panel SHALL display CPU utilization as a percentage
5. THE Metrics_Panel SHALL show throughput as processes per time unit
6. THE Metrics_Panel SHALL update metrics every simulation time step
7. THE Metrics_Panel SHALL reset metrics when simulation is restarted

### Requirement 11: Simulation Time Management

**User Story:** As a user, I want to control simulation timing, so that I can observe the system behavior at different speeds or step through execution.

#### Acceptance Criteria

1. THE Simulator SHALL execute in discrete time steps
2. THE Simulator SHALL support configurable simulation speed (time steps per second)
3. THE Simulator SHALL provide step-by-step execution mode for detailed analysis
4. WHEN in step mode, THE Simulator SHALL advance one time step per user command
5. THE Simulator SHALL display current simulation time prominently
6. THE Simulator SHALL support pause and resume functionality during execution

### Requirement 12: Configuration Management

**User Story:** As an instructor, I want to save and load simulation configurations, so that I can prepare consistent scenarios for different classes.

#### Acceptance Criteria

1. THE Simulator SHALL support saving current configuration to a file
2. THE Simulator SHALL support loading configuration from a file
3. THE Configuration_File SHALL include all process parameters and algorithm selections
4. THE Configuration_File SHALL use a human-readable format (JSON or XML)
5. WHEN loading invalid configuration, THE Simulator SHALL display descriptive error messages
6. THE Simulator SHALL validate configuration parameters before applying them

### Requirement 13: Results Export Functionality

**User Story:** As a student, I want to export simulation results, so that I can include them in reports and assignments.

#### Acceptance Criteria

1. THE Simulator SHALL export Gantt chart as image file (PNG or SVG)
2. THE Simulator SHALL export performance metrics as CSV file
3. THE Simulator SHALL export complete simulation log as text file
4. WHEN exporting, THE Simulator SHALL include timestamp and configuration information
5. THE Simulator SHALL allow user to select export file location
6. IF export fails, THEN THE Simulator SHALL display error message with cause

### Requirement 14: System Architecture and Performance

**User Story:** As a developer, I want the system to be well-architected and performant, so that it can handle complex simulations smoothly.

#### Acceptance Criteria

1. THE Simulator SHALL use C++17 standard features and STL containers
2. THE Simulator SHALL implement Model-View-Controller (MVC) architecture pattern
3. THE Simulator SHALL complete time step processing within 100 milliseconds
4. THE Simulator SHALL support simulations with up to 100 concurrent processes
5. THE Simulator SHALL use Qt framework for all GUI components
6. THE Simulator SHALL build successfully using CMake build system
7. THE Simulator SHALL handle memory allocation failures gracefully

### Requirement 15: Error Handling and Validation

**User Story:** As a user, I want the system to handle errors gracefully, so that I can recover from mistakes without losing work.

#### Acceptance Criteria

1. WHEN invalid input is provided, THE Simulator SHALL display specific error messages
2. THE Simulator SHALL validate process parameters before creating processes
3. IF system resources are exhausted, THEN THE Simulator SHALL notify user and suggest solutions
4. THE Simulator SHALL prevent division by zero in metrics calculations
5. WHEN file operations fail, THE Simulator SHALL provide clear error descriptions
6. THE Simulator SHALL maintain system stability during error conditions
7. THE Simulator SHALL log error events for debugging purposes