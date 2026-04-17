find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(KERNEL_SOURCES
    ${CMAKE_SOURCE_DIR}/src/models/ProcessState.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/ProcessManager.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/Scheduler.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/MemoryManager.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/SyncManager.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/MetricsCollector.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/InputValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/controllers/ConfigurationManager.cpp
)

add_executable(MiniOSIntegrationTests test_integration.cpp ${KERNEL_SOURCES})
target_include_directories(MiniOSIntegrationTests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(MiniOSIntegrationTests Qt6::Core Qt6::Widgets)

add_test(NAME IntegrationTests COMMAND MiniOSIntegrationTests)