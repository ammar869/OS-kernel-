#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <QSplitter>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), central_widget_(nullptr), main_layout_(nullptr), 
      placeholder_label_(nullptr), dashboard_(nullptr), process_table_(nullptr),
      gantt_chart_(nullptr), memory_view_(nullptr), metrics_panel_(nullptr) {
    setupUI();
    setupMenuBar();
    setupStatusBar();
}

MainWindow::~MainWindow() {
    // Qt handles cleanup automatically
}

void MainWindow::setupUI() {
    // Create central widget
    central_widget_ = new QWidget(this);
    setCentralWidget(central_widget_);
    
    // Create main layout
    main_layout_ = new QVBoxLayout(central_widget_);
    
    // Create main splitter for layout
    QSplitter* main_splitter = new QSplitter(Qt::Horizontal, this);
    
    // Left panel - Dashboard
    dashboard_panel_ = new QGroupBox("Control Dashboard", this);
    dashboard_panel_->setFixedWidth(250);
    dashboard_panel_->setMinimumHeight(400);
    
    QVBoxLayout* dashboard_layout = new QVBoxLayout(dashboard_panel_);
    placeholder_label_ = new QLabel("Dashboard Controls\n\n[Start] [Pause] [Stop]\n[Reset] [Step]\n\nSpeed: [====|]\nTime: 00:00\n\nScheduler:\n[FCFS ▼]\n\nMemory Algo:\n[LRU ▼]", this);
    placeholder_label_->setAlignment(Qt::AlignTop);
    placeholder_label_->setStyleSheet("font-family: monospace; padding: 10px; background-color: #f0f0f0; border: 1px solid #ccc;");
    dashboard_layout->addWidget(placeholder_label_);
    
    main_splitter->addWidget(dashboard_panel_);
    
    // Right panel - Main content area
    QWidget* content_area = new QWidget(this);
    QVBoxLayout* content_layout = new QVBoxLayout(content_area);
    
    // Top row - Process Table
    process_table_panel_ = new QGroupBox("Process Table", this);
    process_table_panel_->setMinimumHeight(200);
    
    QVBoxLayout* table_layout = new QVBoxLayout(process_table_panel_);
    QLabel* table_placeholder = new QLabel("PID | State | Priority | Burst | Remaining\n  1  | Ready |    3     |  10   |     8\n  2  | Run   |    1     |   5   |     3\n  3  | Wait  |    2     |   8   |     8", this);
    table_placeholder->setStyleSheet("font-family: monospace; padding: 10px; background-color: #f8f8f8; border: 1px solid #ddd;");
    table_layout->addWidget(table_placeholder);
    
    content_layout->addWidget(process_table_panel_);
    
    // Middle row - Gantt Chart
    gantt_chart_panel_ = new QGroupBox("Gantt Chart", this);
    gantt_chart_panel_->setMinimumHeight(150);
    
    QVBoxLayout* gantt_layout = new QVBoxLayout(gantt_chart_panel_);
    QLabel* gantt_placeholder = new QLabel("P1 ████████\nP2      ██████████\nP3              ████████████\n    0    5    10    15    20    25    30", this);
    gantt_placeholder->setStyleSheet("font-family: monospace; padding: 10px; background-color: #f8f8f8; border: 1px solid #ddd;");
    gantt_layout->addWidget(gantt_placeholder);
    
    content_layout->addWidget(gantt_chart_panel_);
    
    // Bottom row - Memory View and Metrics
    QSplitter* bottom_splitter = new QSplitter(Qt::Horizontal, this);
    
    // Memory View
    memory_view_panel_ = new QGroupBox("Memory View", this);
    memory_view_panel_->setMinimumHeight(200);
    
    QVBoxLayout* memory_layout = new QVBoxLayout(memory_view_panel_);
    QLabel* memory_placeholder = new QLabel("Frame 0: [P1]\nFrame 1: [P2]\nFrame 2: [Free]\nFrame 3: [P1]\nFrame 4: [Free]\nFrame 5: [P3]\n\nPage Table P1:\nVP0 -> PF0\nVP1 -> PF3", this);
    memory_placeholder->setStyleSheet("font-family: monospace; padding: 10px; background-color: #f8f8f8; border: 1px solid #ddd;");
    memory_layout->addWidget(memory_placeholder);
    
    bottom_splitter->addWidget(memory_view_panel_);
    
    // Metrics Panel
    metrics_panel_panel_ = new QGroupBox("Metrics Panel", this);
    metrics_panel_panel_->setMinimumHeight(200);
    
    QVBoxLayout* metrics_layout = new QVBoxLayout(metrics_panel_panel_);
    QLabel* metrics_placeholder = new QLabel("Avg Waiting Time:    5.67 ms\nAvg Turnaround Time: 12.33 ms\nCPU Utilization:     85.4%\nThroughput:          2.1 proc/sec\nMemory Utilization:  60.0%\nPage Faults:         15\nPage Replacements:   8", this);
    metrics_placeholder->setStyleSheet("font-family: monospace; padding: 10px; background-color: #f8f8f8; border: 1px solid #ddd;");
    metrics_layout->addWidget(metrics_placeholder);
    
    bottom_splitter->addWidget(metrics_panel_panel_);
    
    content_layout->addWidget(bottom_splitter);
    
    main_splitter->addWidget(content_area);
    
    // Set splitter proportions
    main_splitter->setSizes({250, 1000});
    
    main_layout_->addWidget(main_splitter);
    
    // Set window properties
    setWindowTitle("Mini OS Kernel Simulator");
    setMinimumSize(1200, 800);
    resize(1400, 900);
}

void MainWindow::setupMenuBar() {
    // File menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New Simulation", this, [this]() {
        QMessageBox::information(this, "New Simulation", "New simulation functionality will be implemented in the next phase.");
    }, QKeySequence::New);
    fileMenu->addAction("&Open Configuration", this, [this]() {
        QMessageBox::information(this, "Open Configuration", "Open configuration functionality will be implemented in the next phase.");
    }, QKeySequence::Open);
    fileMenu->addAction("&Save Configuration", this, [this]() {
        QMessageBox::information(this, "Save Configuration", "Save configuration functionality will be implemented in the next phase.");
    }, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);
    
    // Simulation menu
    QMenu* simulationMenu = menuBar()->addMenu("&Simulation");
    simulationMenu->addAction("&Start", this, [this]() {
        QMessageBox::information(this, "Start Simulation", "Start simulation functionality will be implemented in the next phase.");
    });
    simulationMenu->addAction("&Pause", this, [this]() {
        QMessageBox::information(this, "Pause Simulation", "Pause simulation functionality will be implemented in the next phase.");
    });
    simulationMenu->addAction("&Stop", this, [this]() {
        QMessageBox::information(this, "Stop Simulation", "Stop simulation functionality will be implemented in the next phase.");
    });
    simulationMenu->addAction("&Reset", this, [this]() {
        QMessageBox::information(this, "Reset Simulation", "Reset simulation functionality will be implemented in the next phase.");
    });
    
    // View menu
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("&Dashboard", this, [this]() {
        dashboard_panel_->setVisible(!dashboard_panel_->isVisible());
    });
    viewMenu->addAction("&Process Table", this, [this]() {
        process_table_panel_->setVisible(!process_table_panel_->isVisible());
    });
    viewMenu->addAction("&Gantt Chart", this, [this]() {
        gantt_chart_panel_->setVisible(!gantt_chart_panel_->isVisible());
    });
    viewMenu->addAction("&Memory View", this, [this]() {
        memory_view_panel_->setVisible(!memory_view_panel_->isVisible());
    });
    viewMenu->addAction("&Metrics Panel", this, [this]() {
        metrics_panel_panel_->setVisible(!metrics_panel_panel_->isVisible());
    });
    
    // Help menu
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", this, [this]() {
        QMessageBox::about(this, "About Mini OS Kernel Simulator", 
            "Mini OS Kernel Simulator v1.0\n\n"
            "An educational tool for learning operating system concepts including:\n"
            "• Process Management\n"
            "• CPU Scheduling (FCFS, Round Robin, Priority)\n"
            "• Memory Management with Paging\n"
            "• Synchronization Primitives\n"
            "• Performance Metrics\n\n"
            "Built with C++17 and Qt6");
    });
}

void MainWindow::setupStatusBar() {
    statusBar()->showMessage("Ready - Mini OS Kernel Simulator initialized");
}

#include "MainWindow.moc"