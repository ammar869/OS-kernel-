# Mini OS Kernel Simulator

An educational C++ application that simulates core operating system kernel functionalities including process management, CPU scheduling, memory management, synchronization primitives, and performance metrics collection.

## Features

- **Process Management**: Complete process lifecycle simulation with state management
- **CPU Scheduling**: FCFS, Round Robin, and Priority scheduling algorithms
- **Memory Management**: Paging system with FIFO and LRU replacement algorithms
- **Synchronization**: Semaphores and mutexes for process coordination
- **Performance Metrics**: Real-time collection and analysis of system performance
- **Interactive GUI**: Comprehensive dashboard with visualization components
- **Configuration Management**: Save/load simulation scenarios
- **Export Capabilities**: Results export in multiple formats

## Requirements

- C++17 compatible compiler
- Qt6 (Core, Widgets)
- CMake 3.16 or higher

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./bin/MiniOSKernelSimulator
```

## Architecture

The simulator follows a Model-View-Controller (MVC) architecture:

- **Model**: Kernel components (ProcessManager, Scheduler, MemoryManager, etc.)
- **View**: Qt GUI components (Dashboard, Process Table, Gantt Chart, etc.)
- **Controller**: Application logic (SimulationController, GUIController, etc.)

## Testing

```bash
cd build
make test
```

## License

Educational use only.# OS-kernel-
