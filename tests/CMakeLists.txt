include(FetchContent)

# ── Catch2 v3 ─────────────────────────────────────────────────────────────────
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

# ── RapidCheck ────────────────────────────────────────────────────────────────
FetchContent_Declare(
    rapidcheck
    GIT_REPOSITORY https://github.com/emil-e/rapidcheck.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
# Disable RapidCheck's own tests to keep the build clean
set(RC_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(RC_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(rapidcheck)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# ── Shared kernel sources (no GUI, no main) ───────────────────────────────────
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

# ── Existing integration test target (hand-rolled runner) ─────────────────────
add_executable(MiniOSIntegrationTests test_integration.cpp ${KERNEL_SOURCES})
target_include_directories(MiniOSIntegrationTests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(MiniOSIntegrationTests Qt6::Core Qt6::Widgets)
add_test(NAME IntegrationTests COMMAND MiniOSIntegrationTests)

# ── Catch2 + RapidCheck framework setup / property-based tests ────────────────
add_executable(MiniOSPropertyTests
    test_framework_setup.cpp
    test_process_management.cpp
    ${KERNEL_SOURCES}
)
target_include_directories(MiniOSPropertyTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${rapidcheck_SOURCE_DIR}/extras/catch/include
)
target_link_libraries(MiniOSPropertyTests
    PRIVATE
    Catch2::Catch2WithMain
    rapidcheck
    Qt6::Core
    Qt6::Widgets
)
target_compile_features(MiniOSPropertyTests PRIVATE cxx_std_17)

# Register with CTest using Catch2's helper
include(Catch)
catch_discover_tests(MiniOSPropertyTests)
