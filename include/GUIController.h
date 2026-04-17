#pragma once

#include "SimulationController.h"
#include "Dashboard.h"
#include "ProcessTableWidget.h"
#include "GanttChartWidget.h"
#include "MemoryViewWidget.h"
#include "MetricsWidget.h"
#include <QObject>
#include <QTimer>

/**
 * @brief Connects the GUI widgets to the SimulationController
 * 
 * The GUIController wires all signals/slots between the Dashboard,
 * visualization widgets, and the simulation engine. It drives the
 * periodic refresh of all display widgets.
 */
class GUIController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param sim_controller Pointer to the simulation controller
     * @param dashboard Dashboard widget
     * @param process_table Process table widget
     * @param gantt_chart Gantt chart widget
     * @param memory_view Memory view widget
     * @param metrics_widget Metrics widget
     * @param parent Parent QObject
     */
    GUIController(SimulationController* sim_controller,
                  Dashboard* dashboard,
                  ProcessTableWidget* process_table,
                  GanttChartWidget* gantt_chart,
                  MemoryViewWidget* memory_view,
                  MetricsWidget* metrics_widget,
                  QObject* parent = nullptr);

    ~GUIController() = default;

    /**
     * @brief Set refresh interval for GUI updates
     * @param ms Milliseconds between refreshes
     */
    void setRefreshInterval(int ms);

public slots:
    void onStartSimulation();
    void onPauseSimulation();
    void onStopSimulation();
    void onResetSimulation();
    void onStepSimulation();
    void onSpeedChanged(int speed);
    void onSchedulerChanged(int index);
    void onMemoryAlgorithmChanged(int index);
    void onTimeQuantumChanged(int quantum);
    void refreshGUI();

private:
    void connectSignals();
    void updateAllWidgets();
    void updateGanttChart();

    SimulationController* sim_controller_;
    Dashboard* dashboard_;
    ProcessTableWidget* process_table_;
    GanttChartWidget* gantt_chart_;
    MemoryViewWidget* memory_view_;
    MetricsWidget* metrics_widget_;

    QTimer* refresh_timer_;
    int last_gantt_time_;
};