cmake_minimum_required(VERSION 3.16)
project(MiniOSKernelSimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Tell AUTOMOC to also scan header files
set(CMAKE_AUTOMOC_MACRO_NAMES "Q_OBJECT" "Q_GADGET" "Q_NAMESPACE")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

set(ALL_SOURCES
    src/main.cpp

    # Models
    src/models/ProcessState.cpp
    src/models/Process.cpp
    src/models/MemoryPage.cpp
    src/models/SyncPrimitive.cpp

    # Kernel
    src/kernel/ProcessManager.cpp
    src/kernel/Scheduler.cpp
    src/kernel/MemoryManager.cpp
    src/kernel/SyncManager.cpp
    src/kernel/MetricsCollector.cpp

    # Controllers
    src/controllers/SimulationController.cpp
    src/controllers/ConfigurationManager.cpp
    src/controllers/GUIController.cpp
    src/controllers/ExportController.cpp

    # GUI
    src/gui/MainWindow.cpp
    src/gui/Dashboard.cpp
    src/gui/ProcessTableWidget.cpp
    src/gui/GanttChartWidget.cpp
    src/gui/MemoryViewWidget.cpp
    src/gui/MetricsWidget.cpp

    # Utils
    src/utils/InputValidator.cpp
)

set(ALL_HEADERS
    include/MainWindow.h
    include/Dashboard.h
    include/ProcessTableWidget.h
    include/GanttChartWidget.h
    include/MemoryViewWidget.h
    include/MetricsWidget.h
    include/GUIController.h
)

add_executable(${PROJECT_NAME} ${ALL_SOURCES} ${ALL_HEADERS})
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets)

enable_testing()
add_subdirectory(tests)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)