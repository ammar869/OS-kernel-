#include "ProcessState.h"

const char* processStateToString(ProcessState state) {
    switch (state) {
        case ProcessState::NEW:
            return "NEW";
        case ProcessState::READY:
            return "READY";
        case ProcessState::RUNNING:
            return "RUNNING";
        case ProcessState::WAITING:
            return "WAITING";
        case ProcessState::TERMINATED:
            return "TERMINATED";
        default:
            return "UNKNOWN";
    }
}