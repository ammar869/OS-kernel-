/**
 * @file test_process_management.cpp
 * @brief Property-based tests for process creation and management.
 *
 * Validates: Requirements 1.1, 1.2, 1.6
 *
 * Property 1: Process Creation and Management
 *   For any valid process parameters (priority, burst time, arrival time),
 *   creating a process SHALL result in a unique PID assignment, correct
 *   initial state (NEW), and proper initialization of all process control
 *   block fields with the provided values.
 */

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "ProcessManager.h"
#include "ProcessState.h"

#include <unordered_set>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// Property 1a – Valid PID (>= 1) for any valid parameters
// Validates: Requirements 1.1
// ─────────────────────────────────────────────────────────────────────────────

/**
 * **Validates: Requirements 1.1**
 *
 * For any valid priority [1,10], burst_time [1,100], arrival_time [0,50],
 * createProcess SHALL return a PID >= 1.
 */
TEST_CASE("Property 1a: createProcess returns valid PID for any valid parameters",
          "[process][pbt][req-1.1]")
{
    rc::prop("valid parameters always yield PID >= 1", []() {
        const int priority    = *rc::gen::inRange(1, 11);   // [1, 10]
        const int burst_time  = *rc::gen::inRange(1, 101);  // [1, 100]
        const int arrival_time = *rc::gen::inRange(0, 51);  // [0, 50]

        ProcessManager pm;
        int pid = pm.createProcess(priority, burst_time, arrival_time);

        RC_ASSERT(pid >= 1);
    });
}

// ─────────────────────────────────────────────────────────────────────────────
// Property 1b – All PIDs are unique across multiple creations
// Validates: Requirements 1.1
// ─────────────────────────────────────────────────────────────────────────────

/**
 * **Validates: Requirements 1.1**
 *
 * For any sequence of valid process creations, every returned PID SHALL be
 * unique (no two processes share the same PID).
 */
TEST_CASE("Property 1b: all created PIDs are unique",
          "[process][pbt][req-1.1]")
{
    rc::prop("PIDs are unique across multiple process creations", []() {
        // Create between 2 and 20 processes per run
        const int count = *rc::gen::inRange(2, 21);

        ProcessManager pm;
        std::unordered_set<int> seen_pids;

        for (int i = 0; i < count; ++i) {
            const int priority     = *rc::gen::inRange(1, 11);
            const int burst_time   = *rc::gen::inRange(1, 101);
            const int arrival_time = *rc::gen::inRange(0, 51);

            int pid = pm.createProcess(priority, burst_time, arrival_time);

            // PID must be valid
            RC_ASSERT(pid >= 1);

            // PID must not have been seen before
            RC_ASSERT(seen_pids.find(pid) == seen_pids.end());
            seen_pids.insert(pid);
        }
    });
}

// ─────────────────────────────────────────────────────────────────────────────
// Property 1c – PCB fields match input parameters and initial state is NEW
// Validates: Requirements 1.2, 1.6
// ─────────────────────────────────────────────────────────────────────────────

/**
 * **Validates: Requirements 1.2, 1.6**
 *
 * For any valid parameters, the created process SHALL have:
 *   - state == NEW
 *   - priority  == provided priority
 *   - burst_time == provided burst_time
 *   - remaining_time == burst_time  (full burst remaining at creation)
 *   - arrival_time == provided arrival_time
 *   - A valid PCB accessible via getProcess(pid)
 */
TEST_CASE("Property 1c: PCB fields match input and initial state is NEW",
          "[process][pbt][req-1.2][req-1.6]")
{
    rc::prop("created PCB has correct initial state and field values", []() {
        const int priority     = *rc::gen::inRange(1, 11);
        const int burst_time   = *rc::gen::inRange(1, 101);
        const int arrival_time = *rc::gen::inRange(0, 51);

        ProcessManager pm;
        int pid = pm.createProcess(priority, burst_time, arrival_time);

        RC_ASSERT(pid >= 1);

        // Requirement 1.6: PCB must exist and be retrievable
        ProcessControlBlock* pcb = pm.getProcess(pid);
        RC_ASSERT(pcb != nullptr);

        // Requirement 1.2: initial state must be NEW
        RC_ASSERT(pcb->state == ProcessState::NEW);

        // Requirement 1.2: PCB fields must match the provided values
        RC_ASSERT(pcb->pid         == pid);
        RC_ASSERT(pcb->priority    == priority);
        RC_ASSERT(pcb->burst_time  == burst_time);
        RC_ASSERT(pcb->arrival_time == arrival_time);

        // remaining_time is initialised to burst_time at creation
        RC_ASSERT(pcb->remaining_time == burst_time);
    });
}
