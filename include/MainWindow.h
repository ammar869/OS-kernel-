#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

#include "SimulationController.h"
#include "Dashboard.h"
#include "ProcessTableWidget.h"
#include "GanttChartWidget.h"
#include "MemoryViewWidget.h"
#include "MetricsWidget.h"
#include "GUIController.h"
#include "ExportController.h"

/**
 * @brief Main application window — integrates all GUI components
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onNewSimulation();
    void onOpenConfiguration();
    void onSaveConfiguration();
    void onExportGantt();
    void onExportMetrics();
    void onExportLog();
    void onAbout();

private:
    void setupComponents();
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();

    // Core components
    SimulationController* sim_controller_;
    ExportController*     export_controller_;
    GUIController*        gui_controller_;

    // GUI widgets
    Dashboard*            dashboard_;
    ProcessTableWidget*   process_table_;
    GanttChartWidget*     gantt_chart_;
    MemoryViewWidget*     memory_view_;
    MetricsWidget*        metrics_widget_;
};