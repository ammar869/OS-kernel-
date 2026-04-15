cmake_minimum_required(VERSION 3.16)
project(MiniOSKernelSimulator VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add missing source files that exist
set(EXISTING_SOURCES
    src/models/ProcessState.cpp
    src/models/Process.cpp
    src/models/MemoryPage.cpp
    src/models/SyncPrimitive.cpp
    src/kernel/ProcessManager.cpp
    src/kernel/Scheduler.cpp
    src/kernel/MemoryManager.cpp
    src/kernel/SyncManager.cpp
    src/kernel/MetricsCollector.cpp
    src/gui/MainWindow.cpp
    src/utils/InputValidator.cpp
)

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Debug/Release configurations
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Source files
set(KERNEL_SOURCES
    src/kernel/ProcessManager.cpp
    src/kernel/Scheduler.cpp
    src/kernel/MemoryManager.cpp
    src/kernel/SyncManager.cpp
    src/kernel/MetricsCollector.cpp
    src/kernel/KernelCore.cpp
)

set(GUI_SOURCES
    src/gui/MainWindow.cpp
    src/gui/Dashboard.cpp
    src/gui/ProcessTableWidget.cpp
    src/gui/GanttChartWidget.cpp
    src/gui/MemoryViewWidget.cpp
    src/gui/MetricsWidget.cpp
)

set(CONTROLLER_SOURCES
    src/controllers/SimulationController.cpp
    src/controllers/GUIController.cpp
    src/controllers/ConfigurationManager.cpp
    src/controllers/ExportController.cpp
)

set(MODEL_SOURCES
    src/models/Process.cpp
    src/models/MemoryPage.cpp
    src/models/SyncPrimitive.cpp
)

set(UTILS_SOURCES
    src/utils/Timer.cpp
    src/utils/Config.cpp
    src/utils/InputValidator.cpp
)

# Main executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    ${EXISTING_SOURCES}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets)

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(tests)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "MiniOSKernelSimulator")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Educational OS Kernel Simulator")
set(CPACK_PACKAGE_VENDOR "Educational Software")
include(CPack)