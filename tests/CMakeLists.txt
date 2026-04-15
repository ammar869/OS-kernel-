# Test configuration
find_package(Qt6 REQUIRED COMPONENTS Test)

# Test sources
set(TEST_SOURCES
    test_ProcessManager.cpp
    test_Scheduler.cpp
    test_MemoryManager.cpp
    test_SyncManager.cpp
    test_MetricsCollector.cpp
    test_GUI.cpp
)

# Create test executable
add_executable(MiniOSTests ${TEST_SOURCES})

# Link libraries
target_link_libraries(MiniOSTests 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Test
)

# Include source directories for testing
target_include_directories(MiniOSTests PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

# Add test
add_test(NAME MiniOSKernelSimulatorTests COMMAND MiniOSTests)