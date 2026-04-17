#include "GUIController.h"
#include <QMessageBox>

GUIController::GUIController(SimulationController* sim_controller,
                             Dashboard* dashboard,
                             ProcessTableWidget* process_table,
                             GanttChartWidget* gantt_chart,
                             MemoryViewWidget* memory_view,
                             MetricsWidget* metrics_widget,
                             QObject* parent)
    : QObject(parent),
      sim_controller_(sim_controller),
      dashboard_(dashboard),
      process_table_(process_table),
      gantt_chart_(gantt_chart),
      memory_view_(memory_view),
      metrics_widget_(metrics_widget),
      last_gantt_time_(-1)
{
    refresh_timer_ = new QTimer(this);
    connect(refresh_timer_, &QTimer::timeout, this, &GUIController::refreshGUI);
    refresh_timer_->setInterval(200); // 5 fps refresh

    connectSignals();
}

void GUIController::setRefreshInterval(int ms) {
    refresh_timer_->setInterval(ms);
}

void GUIController::connectSignals() {
    // Dashboard -> GUIController
    connect(dashboard_, &Dashboard::simulationStartRequested, this, &GUIController::onStartSimulation);
    connect(dashboard_, &Dashboard::simulationPaused,         this, &GUIController::onPauseSimulation);
    connect(dashboard_, &Dashboard::simulationStopped,        this, &GUIController::onStopSimulation);
    connect(dashboard_, &Dashboard::simulationReset,          this, &GUIController::onResetSimulation);
    connect(dashboard_, &Dashboard::stepRequested,            this, &GUIController::onStepSimulation);
    connect(dashboard_, &Dashboard::speedChanged,             this, &GUIController::onSpeedChanged);
    connect(dashboard_, &Dashboard::schedulerChanged,         this, &GUIController::onSchedulerChanged);
    connect(dashboard_, &Dashboard::memoryAlgorithmChanged,   this, &GUIController::onMemoryAlgorithmChanged);
    connect(dashboard_, &Dashboard::timeQuantumChanged,       this, &GUIController::onTimeQuantumChanged);

    // Process table -> memory view (show page table for selected process)
    connect(process_table_, &ProcessTableWidget::processSelected,
            memory_view_, &MemoryViewWidget::setSelectedProcess);

    // Memory view frame click -> highlight
    connect(memory_view_, &MemoryViewWidget::frameClicked, this, [this](int frame_id) {
        memory_view_->highlightFrame(frame_id);
    });
}

void GUIController::onStartSimulation() {
    if (!sim_controller_) return;

    // Build config from dashboard
    SimulationConfig config;
    config.simulation_speed = dashboard_->getSimulationSpeed();
    config.time_quantum = dashboard_->getTimeQuantum();
    config.step_mode = false;

    int sched_idx = dashboard_->getSchedulingAlgorithm();
    config.scheduling_algo = static_cast<SchedulingAlgorithm>(sched_idx);

    int mem_idx = dashboard_->getMemoryAlgorithm();
    config.memory_config.replacement_algo = static_cast<ReplacementAlgorithm>(mem_idx);

    sim_controller_->initialize(config);
    sim_controller_->start();

    dashboard_->setSimulationRunning(true);
    refresh_timer_->start();
    last_gantt_time_ = -1;
}

void GUIController::onPauseSimulation() {
    if (!sim_controller_) return;
    sim_controller_->pause();
    dashboard_->setSimulationRunning(false);
}

void GUIController::onStopSimulation() {
    if (!sim_controller_) return;
    sim_controller_->stop();
    dashboard_->setSimulationRunning(false);
    refresh_timer_->stop();
    updateAllWidgets();
}

void GUIController::onResetSimulation() {
    if (!sim_controller_) return;
    sim_controller_->reset();
    dashboard_->setSimulationRunning(false);
    refresh_timer_->stop();

    process_table_->clearProcesses();
    gantt_chart_->clearChart();
    memory_view_->clearMemory();
    metrics_widget_->resetMetrics();
    last_gantt_time_ = -1;
}

void GUIController::onStepSimulation() {
    if (!sim_controller_) return;

    // Enable step mode and execute one step
    sim_controller_->setStepMode(true);
    if (sim_controller_->getState() == SimulationController::SimulationState::STOPPED) {
        SimulationConfig config;
        config.step_mode = true;
        config.simulation_speed = 1;
        config.time_quantum = dashboard_->getTimeQuantum();
        config.scheduling_algo = static_cast<SchedulingAlgorithm>(dashboard_->getSchedulingAlgorithm());
        config.memory_config.replacement_algo = static_cast<ReplacementAlgorithm>(dashboard_->getMemoryAlgorithm());
        sim_controller_->initialize(config);
        sim_controller_->start();
    }
    sim_controller_->executeStep();
    updateAllWidgets();
}

void GUIController::onSpeedChanged(int speed) {
    if (sim_controller_) {
        sim_controller_->setSimulationSpeed(speed);
    }
}

void GUIController::onSchedulerChanged(int index) {
    if (sim_controller_) {
        sim_controller_->setSchedulingAlgorithm(
            static_cast<SchedulingAlgorithm>(index),
            dashboard_->getTimeQuantum());
    }
}

void GUIController::onMemoryAlgorithmChanged(int index) {
    if (sim_controller_) {
        sim_controller_->getMemoryManager()->setReplacementAlgorithm(
            static_cast<ReplacementAlgorithm>(index));
    }
}

void GUIController::onTimeQuantumChanged(int quantum) {
    if (sim_controller_) {
        sim_controller_->setSchedulingAlgorithm(
            static_cast<SchedulingAlgorithm>(dashboard_->getSchedulingAlgorithm()),
            quantum);
    }
}

void GUIController::refreshGUI() {
    if (!sim_controller_) return;

    // Stop refreshing if simulation stopped
    auto state = sim_controller_->getState();
    if (state == SimulationController::SimulationState::STOPPED) {
        refresh_timer_->stop();
        dashboard_->setSimulationRunning(false);
    }

    updateAllWidgets();
}

void GUIController::updateAllWidgets() {
    if (!sim_controller_) return;

    int current_time = sim_controller_->getCurrentTime();
    dashboard_->setSimulationTime(current_time);

    // Update process table
    auto processes = sim_controller_->getAllProcesses();
    process_table_->updateProcesses(processes);

    // Highlight running process
    int running_pid = -1;
    for (auto* pcb : processes) {
        if (pcb->state == ProcessState::RUNNING) {
            running_pid = pcb->pid;
            break;
        }
    }
    process_table_->highlightRunningProcess(running_pid);

    // Update Gantt chart
    updateGanttChart();

    // Update memory view
    auto* mm = sim_controller_->getMemoryManager();
    if (mm) {
        memory_view_->updateMemoryFrames(mm->getPageFrames());
        memory_view_->setMemoryConfiguration(mm->getConfiguration());

        // Update page tables for all processes
        for (auto* pcb : processes) {
            const auto* pt = mm->getPageTable(pcb->pid);
            if (pt) {
                memory_view_->updatePageTable(pcb->pid, *pt);
            }
        }
    }

    // Update metrics
    SystemMetrics metrics = sim_controller_->getSystemMetrics();
    metrics_widget_->updateMetrics(metrics);
}

void GUIController::updateGanttChart() {
    int current_time = sim_controller_->getCurrentTime();
    if (current_time <= last_gantt_time_) return;

    // Add a block for the currently running process
    auto processes = sim_controller_->getAllProcesses();
    for (auto* pcb : processes) {
        if (pcb->state == ProcessState::RUNNING) {
            gantt_chart_->addTimeBlock(pcb->pid, current_time - 1, current_time,
                                       QString("P%1").arg(pcb->pid));
            break;
        }
    }
    last_gantt_time_ = current_time;
}