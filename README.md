# Mini OS Kernel Simulator

An educational C++ application that simulates core operating system kernel functionalities with a full Qt6 GUI.

## Features

| Module | Details |
|--------|---------|
| **Process Management** | Full lifecycle (NEW → READY → RUNNING → WAITING → TERMINATED), PCB, thread-safe |
| **CPU Scheduling** | FCFS, Round Robin (configurable quantum), Priority (preemptive) |
| **Memory Management** | Paging system, FIFO & LRU page replacement, page fault handling |
| **Synchronization** | Counting semaphores, mutexes, deadlock detection |
| **Performance Metrics** | Waiting time, turnaround time, response time, CPU utilization, throughput |
| **GUI** | Dashboard, Process Table, Gantt Chart, Memory View, Metrics Panel |
| **Configuration** | Save/load JSON configs |
| **Export** | Gantt chart PNG, metrics CSV, simulation log TXT |

## Requirements

- C++17 compiler (GCC 10+, MSVC 2019+, Clang 12+)
- Qt6 (Core, Widgets)
- CMake 3.16+

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Run

```bash
./bin/MiniOSKernelSimulator
```

## Run Tests

```bash
cd build
ctest --output-on-failure
```

Or directly:
```bash
g++ -std=c++17 -I include -o test_integration tests/test_integration.cpp \
    src/models/ProcessState.cpp src/kernel/ProcessManager.cpp \
    src/kernel/Scheduler.cpp src/kernel/MemoryManager.cpp \
    src/kernel/SyncManager.cpp src/kernel/MetricsCollector.cpp \
    src/utils/InputValidator.cpp src/controllers/ConfigurationManager.cpp
./test_integration
```

## Architecture

```
GUI (Qt6 Widgets)
  └── GUIController
        └── SimulationController
              ├── ProcessManager   (process lifecycle)
              ├── Scheduler        (FCFS / Round Robin / Priority)
              ├── MemoryManager    (paging, FIFO/LRU)
              ├── SyncManager      (semaphores, mutexes)
              └── MetricsCollector (performance stats)
```

## Project Structure

```
├── include/          # All header files
├── src/
│   ├── kernel/       # Core OS simulation components
│   ├── controllers/  # SimulationController, GUIController, etc.
│   ├── gui/          # Qt widget implementations
│   ├── models/       # Data model utilities
│   └── utils/        # InputValidator, Timer
├── tests/            # Integration test suite (65 tests)
├── resources/        # Icons and assets
└── CMakeLists.txt
```

## Usage

1. **Add Processes** — Set priority, burst time, arrival time in the Dashboard panel
2. **Select Algorithm** — Choose CPU scheduler and memory replacement algorithm
3. **Start** — Click Start or use Step mode for step-by-step execution
4. **Observe** — Watch the Process Table, Gantt Chart, Memory View, and Metrics Panel update in real time
5. **Export** — Use the Export menu to save results

## License

Educational use only.