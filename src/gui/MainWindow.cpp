#include "MainWindow.h"
#include "GUIController.h"
#include "ExportController.h"
#include "ConfigurationManager.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QScrollArea>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupComponents();
    setupUI();
    setupMenuBar();
    setupStatusBar();
}

MainWindow::~MainWindow() {}

void MainWindow::setupComponents() {
    qDebug() << "[MW] Creating SimulationController";
    sim_controller_    = new SimulationController();
    qDebug() << "[MW] Creating ExportController";
    export_controller_ = new ExportController();
    qDebug() << "[MW] Creating Dashboard";
    dashboard_         = new Dashboard(this);
    qDebug() << "[MW] Creating ProcessTableWidget";
    process_table_     = new ProcessTableWidget(this);
    qDebug() << "[MW] Creating GanttChartWidget";
    gantt_chart_       = new GanttChartWidget(this);
    qDebug() << "[MW] Creating MemoryViewWidget";
    memory_view_       = new MemoryViewWidget(this);
    qDebug() << "[MW] Creating MetricsWidget";
    metrics_widget_    = new MetricsWidget(this);
    qDebug() << "[MW] Creating GUIController";
    gui_controller_    = new GUIController(
        sim_controller_, dashboard_, process_table_,
        gantt_chart_, memory_view_, metrics_widget_, this);
    qDebug() << "[MW] Connecting signals";
    connect(metrics_widget_, &MetricsWidget::exportRequested,
            this, &MainWindow::onExportMetrics);
    qDebug() << "[MW] setupComponents done";
}

void MainWindow::setupUI() {
    setWindowTitle("Mini OS Kernel Simulator");
    setMinimumSize(1200, 800);
    resize(1500, 950);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout* root = new QHBoxLayout(central);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(4);

    // Left panel: Dashboard + Metrics
    QWidget* left_panel = new QWidget(this);
    left_panel->setFixedWidth(260);
    QVBoxLayout* left_layout = new QVBoxLayout(left_panel);
    left_layout->setContentsMargins(0, 0, 0, 0);
    left_layout->setSpacing(4);
    left_layout->addWidget(dashboard_);

    QScrollArea* metrics_scroll = new QScrollArea(this);
    metrics_scroll->setWidget(metrics_widget_);
    metrics_scroll->setWidgetResizable(true);
    metrics_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    left_layout->addWidget(metrics_scroll);
    root->addWidget(left_panel);

    // Right panel: Process Table + Gantt + Memory
    QSplitter* right_splitter = new QSplitter(Qt::Vertical, this);

    QGroupBox* table_group = new QGroupBox("Process Table", this);
    QVBoxLayout* table_layout = new QVBoxLayout(table_group);
    table_layout->addWidget(process_table_);
    right_splitter->addWidget(table_group);

    QGroupBox* gantt_group = new QGroupBox("Gantt Chart", this);
    QVBoxLayout* gantt_layout = new QVBoxLayout(gantt_group);
    gantt_layout->addWidget(gantt_chart_);
    right_splitter->addWidget(gantt_group);

    QGroupBox* mem_group = new QGroupBox("Memory View", this);
    QVBoxLayout* mem_layout = new QVBoxLayout(mem_group);
    mem_layout->addWidget(memory_view_);
    right_splitter->addWidget(mem_group);

    right_splitter->setSizes({250, 200, 300});
    root->addWidget(right_splitter, 1);
}

void MainWindow::setupMenuBar() {
    QMenu* file_menu = menuBar()->addMenu("&File");
    auto* act_new = file_menu->addAction("&New Simulation");
    act_new->setShortcut(QKeySequence::New);
    connect(act_new, &QAction::triggered, this, &MainWindow::onNewSimulation);

    auto* act_open = file_menu->addAction("&Open Configuration...");
    act_open->setShortcut(QKeySequence::Open);
    connect(act_open, &QAction::triggered, this, &MainWindow::onOpenConfiguration);

    auto* act_save = file_menu->addAction("&Save Configuration...");
    act_save->setShortcut(QKeySequence::Save);
    connect(act_save, &QAction::triggered, this, &MainWindow::onSaveConfiguration);

    file_menu->addSeparator();
    auto* act_exit = file_menu->addAction("E&xit");
    act_exit->setShortcut(QKeySequence::Quit);
    connect(act_exit, &QAction::triggered, this, &QWidget::close);

    QMenu* export_menu = menuBar()->addMenu("&Export");
    connect(export_menu->addAction("Export &Gantt Chart (PNG)..."),
            &QAction::triggered, this, &MainWindow::onExportGantt);
    connect(export_menu->addAction("Export &Metrics (CSV)..."),
            &QAction::triggered, this, &MainWindow::onExportMetrics);
    connect(export_menu->addAction("Export Simulation &Log..."),
            &QAction::triggered, this, &MainWindow::onExportLog);

    QMenu* help_menu = menuBar()->addMenu("&Help");
    connect(help_menu->addAction("&About"), &QAction::triggered,
            this, &MainWindow::onAbout);
}

void MainWindow::setupStatusBar() {
    statusBar()->showMessage("Ready — Mini OS Kernel Simulator");
}

void MainWindow::onNewSimulation() {
    auto reply = QMessageBox::question(this, "New Simulation",
        "Reset the current simulation?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        gui_controller_->onResetSimulation();
        statusBar()->showMessage("New simulation ready.");
    }
}

void MainWindow::onOpenConfiguration() {
    QString filename = QFileDialog::getOpenFileName(
        this, "Open Configuration", "", "JSON Files (*.json);;All Files (*)");
    if (filename.isEmpty()) return;

    ConfigurationManager cm;
    SimulationConfig config;
    if (cm.loadConfiguration(filename.toStdString(), config)) {
        dashboard_->setSchedulingAlgorithm(static_cast<int>(config.scheduling_algo));
        dashboard_->setTimeQuantum(config.time_quantum);
        dashboard_->setSimulationSpeed(config.simulation_speed);
        dashboard_->setMemoryAlgorithm(static_cast<int>(config.memory_config.replacement_algo));
        statusBar()->showMessage("Configuration loaded: " + filename);
    } else {
        QMessageBox::critical(this, "Load Error",
            QString("Failed to load:\n%1").arg(QString::fromStdString(cm.getLastError())));
    }
}

void MainWindow::onSaveConfiguration() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Save Configuration", "simulation_config.json",
        "JSON Files (*.json);;All Files (*)");
    if (filename.isEmpty()) return;

    SimulationConfig config;
    config.scheduling_algo = static_cast<SchedulingAlgorithm>(dashboard_->getSchedulingAlgorithm());
    config.time_quantum     = dashboard_->getTimeQuantum();
    config.simulation_speed = dashboard_->getSimulationSpeed();
    config.memory_config.replacement_algo =
        static_cast<ReplacementAlgorithm>(dashboard_->getMemoryAlgorithm());

    ConfigurationManager cm;
    if (cm.saveConfiguration(config, filename.toStdString())) {
        statusBar()->showMessage("Configuration saved: " + filename);
    } else {
        QMessageBox::critical(this, "Save Error",
            QString("Failed to save:\n%1").arg(QString::fromStdString(cm.getLastError())));
    }
}

void MainWindow::onExportGantt() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Export Gantt Chart", "gantt_chart.png",
        "PNG Images (*.png);;All Files (*)");
    if (filename.isEmpty()) return;

    if (export_controller_->exportGanttChart(gantt_chart_, filename)) {
        statusBar()->showMessage("Gantt chart exported: " + filename);
    } else {
        QMessageBox::critical(this, "Export Error",
            "Failed:\n" + export_controller_->getLastError());
    }
}

void MainWindow::onExportMetrics() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Export Metrics", "metrics.csv",
        "CSV Files (*.csv);;All Files (*)");
    if (filename.isEmpty()) return;

    SystemMetrics metrics = sim_controller_->getSystemMetrics();
    if (export_controller_->exportMetricsCSV(metrics, filename)) {
        statusBar()->showMessage("Metrics exported: " + filename);
    } else {
        QMessageBox::critical(this, "Export Error",
            "Failed:\n" + export_controller_->getLastError());
    }
}

void MainWindow::onExportLog() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Export Simulation Log", "simulation_log.txt",
        "Text Files (*.txt);;All Files (*)");
    if (filename.isEmpty()) return;

    auto* mc = sim_controller_->getMetricsCollector();
    std::string report = mc ? mc->generateReport() : "No simulation data.";

    if (export_controller_->exportSimulationLog(report, filename)) {
        statusBar()->showMessage("Log exported: " + filename);
    } else {
        QMessageBox::critical(this, "Export Error",
            "Failed:\n" + export_controller_->getLastError());
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "About Mini OS Kernel Simulator",
        "<h2>Mini OS Kernel Simulator</h2>"
        "<p>Version 1.0.0 — Built with C++17 and Qt6</p>"
        "<ul>"
        "<li>Process Management (FCFS, Round Robin, Priority)</li>"
        "<li>Memory Management (Paging, FIFO/LRU)</li>"
        "<li>Synchronization (Semaphores, Mutexes)</li>"
        "<li>Real-time Performance Metrics</li>"
        "</ul>");
}
