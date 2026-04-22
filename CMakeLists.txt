cmake_minimum_required(VERSION 3.16)
project(MiniOSKernelSimulator VERSION 1.0.0 LANGUAGES CXX)

# ── C++ Standard ──────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Qt Auto-tools (must be set BEFORE find_package) ───────────────────────────
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ── Find Qt6 ──────────────────────────────────────────────────────────────────
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# ── Output directories ────────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ── Sources ───────────────────────────────────────────────────────────────────
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

# Headers with Q_OBJECT — listed explicitly so AUTOMOC scans them
set(ALL_HEADERS
    include/MainWindow.h
    include/Dashboard.h
    include/ProcessTableWidget.h
    include/GanttChartWidget.h
    include/MemoryViewWidget.h
    include/MetricsWidget.h
    include/GUIController.h
)

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${ALL_SOURCES} ${ALL_HEADERS})

# ── Include paths ─────────────────────────────────────────────────────────────
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# ── Link Qt modules ───────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# ── Tests ─────────────────────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests)

# ── Install ───────────────────────────────────────────────────────────────────
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
