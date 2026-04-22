/**
 * @file test_framework_setup.cpp
 * @brief Catch2 + RapidCheck framework smoke tests.
 *
 * Verifies that:
 *  - Catch2 v3 is correctly linked and TEST_CASE macros work.
 *  - RapidCheck is correctly linked and rc::check() works.
 *  - Kernel headers compile cleanly under the test build.
 *
 * These tests are intentionally minimal; they exist to confirm the
 * testing infrastructure is functional before property tests are added
 * in subsequent tasks.
 */

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "test_utils.h"

#include "ProcessManager.h"
#include "Scheduler.h"
#include "MemoryManager.h"
#include "SyncManager.h"
#include "MetricsCollector.h"
#include "InputValidator.h"
#include "ProcessState.h"
#include "MemoryTypes.h"
#include "SimulationConfig.h"

// ─────────────────────────────────────────────────────────────────────────────
// 1. Catch2 smoke test
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("Catch2 framework is operational", "[framework][smoke]")
{
    SECTION("basic arithmetic")
    {
        REQUIRE(1 + 1 == 2);
        REQUIRE(10 - 3 == 7);
    }

    SECTION("boolean assertions")
    {
        REQUIRE(true);
        CHECK_FALSE(false);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 2. Kernel header compilation smoke tests
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("Kernel objects can be constructed", "[framework][smoke][kernel]")
{
    SECTION("ProcessManager default construction")
    {
        ProcessManager pm;
        REQUIRE(pm.getProcessCount() == 0);
    }

    SECTION("MemoryManager default construction")
    {
        MemoryManager mm;
        REQUIRE(mm.getFreeFrameCount() >= 0);
    }

    SECTION("SyncManager default construction")
    {
        SyncManager sm;
        REQUIRE(sm.getSemaphoreCount() == 0);
        REQUIRE(sm.getMutexCount() == 0);
    }

    SECTION("FCFSScheduler construction")
    {
        FCFSScheduler fcfs;
        REQUIRE(std::string(fcfs.getAlgorithmName()) == "FCFS");
    }

    SECTION("RoundRobinScheduler construction with quantum")
    {
        RoundRobinScheduler rr(4);
        REQUIRE(rr.getTimeQuantum() == 4);
    }

    SECTION("PriorityScheduler construction")
    {
        PriorityScheduler ps;
        REQUIRE(std::string(ps.getAlgorithmName()) == "Priority");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 3. Test utilities smoke tests
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("Test utilities produce valid objects", "[framework][smoke][utils]")
{
    SECTION("makeProcess creates PCB with correct fields")
    {
        auto pcb = makeProcess(1, 3, 10, 0);
        REQUIRE(pcb.pid == 1);
        REQUIRE(pcb.priority == 3);
        REQUIRE(pcb.burst_time == 10);
        REQUIRE(pcb.remaining_time == 10);
        REQUIRE(pcb.arrival_time == 0);
        REQUIRE(pcb.state == ProcessState::NEW);
    }

    SECTION("makeReadyProcess creates PCB in READY state")
    {
        auto pcb = makeReadyProcess(2, 5, 8, 1);
        REQUIRE(pcb.state == ProcessState::READY);
    }

    SECTION("buildReadyQueue populates ProcessManager")
    {
        ProcessManager pm;
        auto pids = buildReadyQueue(pm, 5);
        REQUIRE(pids.size() == 5);
        REQUIRE(pm.getProcessCount() == 5);
        REQUIRE(pm.getProcessCountByState(ProcessState::READY) == 5);
    }

    SECTION("makeMemoryManager creates manager with correct frame count")
    {
        auto mm = makeMemoryManager(8);
        REQUIRE(mm.getFreeFrameCount() == 8);
    }

    SECTION("makeSyncManager creates semaphores and mutexes")
    {
        std::vector<int> sem_ids, mutex_ids;
        auto sm = makeSyncManager(2, 3, sem_ids, mutex_ids);
        REQUIRE(sem_ids.size() == 2);
        REQUIRE(mutex_ids.size() == 3);
        REQUIRE(sm.getSemaphoreCount() == 2);
        REQUIRE(sm.getMutexCount() == 3);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 4. RapidCheck smoke test — process PIDs are always positive
// ─────────────────────────────────────────────────────────────────────────────

RC_BOOST_PROP(process_pids_are_always_positive, ())
{
    // This is intentionally a no-op placeholder that confirms rc::check compiles.
    // Real property tests are added in tasks 2.2 – 21.3.
}

TEST_CASE("RapidCheck: process PIDs are always positive", "[framework][smoke][pbt]")
{
    // Generate valid process parameters and verify the created PID > 0.
    rc::prop("created process PID is always positive", []() {
        // Generate priority in [1,10], burst in [1,100], arrival in [0,50]
        const int priority = *rc::gen::inRange(1, 11);   // [1, 10]
        const int burst    = *rc::gen::inRange(1, 101);  // [1, 100]
        const int arrival  = *rc::gen::inRange(0, 51);   // [0, 50]

        ProcessManager pm;
        int pid = pm.createProcess(priority, burst, arrival);

        RC_ASSERT(pid > 0);
    });
}

TEST_CASE("RapidCheck: invalid process parameters yield PID == -1", "[framework][smoke][pbt]")
{
    rc::prop("priority out of range is rejected", []() {
        // Generate priority strictly outside [1,10]
        const int bad_priority = *rc::gen::oneOf(
            rc::gen::inRange(-100, 1),   // < 1
            rc::gen::inRange(11, 200)    // > 10
        );
        const int burst   = *rc::gen::inRange(1, 101);
        const int arrival = *rc::gen::inRange(0, 51);

        ProcessManager pm;
        int pid = pm.createProcess(bad_priority, burst, arrival);

        RC_ASSERT(pid == -1);
    });
}
