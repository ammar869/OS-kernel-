#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), central_widget_(nullptr), main_layout_(nullptr), placeholder_label_(nullptr) {
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
    
    // Add placeholder content
    placeholder_label_ = new QLabel("Mini OS Kernel Simulator - Coming Soon!", this);
    placeholder_label_->setAlignment(Qt::AlignCenter);
    placeholder_label_->setStyleSheet("font-size: 18px; font-weight: bold; color: #333;");
    
    main_layout_->addWidget(placeholder_label_);
    
    // Set window properties
    setWindowTitle("Mini OS Kernel Simulator");
    setMinimumSize(1200, 800);
    resize(1400, 900);
}

void MainWindow::setupMenuBar() {
    // File menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New Simulation", this, [this]() {
        // TODO: Implement new simulation
    }, QKeySequence::New);
    fileMenu->addAction("&Open Configuration", this, [this]() {
        // TODO: Implement open configuration
    }, QKeySequence::Open);
    fileMenu->addAction("&Save Configuration", this, [this]() {
        // TODO: Implement save configuration
    }, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);
    
    // Simulation menu
    QMenu* simulationMenu = menuBar()->addMenu("&Simulation");
    simulationMenu->addAction("&Start", this, [this]() {
        // TODO: Implement start simulation
    });
    simulationMenu->addAction("&Pause", this, [this]() {
        // TODO: Implement pause simulation
    });
    simulationMenu->addAction("&Stop", this, [this]() {
        // TODO: Implement stop simulation
    });
    simulationMenu->addAction("&Reset", this, [this]() {
        // TODO: Implement reset simulation
    });
    
    // View menu
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("&Dashboard", this, [this]() {
        // TODO: Show/hide dashboard
    });
    viewMenu->addAction("&Process Table", this, [this]() {
        // TODO: Show/hide process table
    });
    viewMenu->addAction("&Gantt Chart", this, [this]() {
        // TODO: Show/hide Gantt chart
    });
    viewMenu->addAction("&Memory View", this, [this]() {
        // TODO: Show/hide memory view
    });
    viewMenu->addAction("&Metrics Panel", this, [this]() {
        // TODO: Show/hide metrics panel
    });
    
    // Help menu
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", this, [this]() {
        // TODO: Show about dialog
    });
}

void MainWindow::setupStatusBar() {
    statusBar()->showMessage("Ready");
}

#include "MainWindow.moc"