#include "Dashboard.h"
#include <QMessageBox>

Dashboard::Dashboard(QWidget* parent)
    : QWidget(parent), current_time_(0), simulation_running_(false) {
    setupUI();
}

void Dashboard::setSimulationTime(int time) {
    current_time_ = time;
    updateTimeDisplay();
}

void Dashboard::setSimulationSpeed(int speed) {
    if (speed >= 1 && speed <= 10) {
        speed_slider_->setValue(speed);
    }
}

int Dashboard::getSimulationSpeed() const {
    return speed_slider_->value();
}

void Dashboard::setSchedulingAlgorithm(int algorithm) {
    if (algorithm >= 0 && algorithm < scheduler_combo_->count()) {
        scheduler_combo_->setCurrentIndex(algorithm);
    }
}

int Dashboard::getSchedulingAlgorithm() const {
    return scheduler_combo_->currentIndex();
}

void Dashboard::setMemoryAlgorithm(int algorithm) {
    if (algorithm >= 0 && algorithm < memory_algo_combo_->count()) {
        memory_algo_combo_->setCurrentIndex(algorithm);
    }
}

int Dashboard::getMemoryAlgorithm() const {
    return memory_algo_combo_->currentIndex();
}

void Dashboard::setTimeQuantum(int quantum) {
    time_quantum_spin_->setValue(quantum);
}

int Dashboard::getTimeQuantum() const {
    return time_quantum_spin_->value();
}

void Dashboard::setSimulationRunning(bool running) {
    simulation_running_ = running;
    
    start_button_->setEnabled(!running);
    pause_button_->setEnabled(running);
    stop_button_->setEnabled(running);
    step_button_->setEnabled(!running);
    
    // Disable algorithm changes during simulation
    scheduler_combo_->setEnabled(!running);
    memory_algo_combo_->setEnabled(!running);
    time_quantum_spin_->setEnabled(!running);
}

void Dashboard::onStartSimulation() {
    emit simulationStartRequested();
}

void Dashboard::onPauseSimulation() {
    emit simulationPaused();
}

void Dashboard::onStopSimulation() {
    emit simulationStopped();
}

void Dashboard::onResetSimulation() {
    emit simulationReset();
    setSimulationTime(0);
}

void Dashboard::onStepSimulation() {
    emit stepRequested();
}

void Dashboard::onSpeedChanged(int speed) {
    speed_label_->setText(QString("Speed: %1 steps/sec").arg(speed));
    emit speedChanged(speed);
}

void Dashboard::onSchedulerChanged(int index) {
    // Enable/disable time quantum based on scheduler
    bool is_round_robin = (index == 1); // Assuming Round Robin is index 1
    time_quantum_spin_->setEnabled(is_round_robin && !simulation_running_);
    quantum_label_->setEnabled(is_round_robin);
    
    emit schedulerChanged(index);
}

void Dashboard::onMemoryAlgorithmChanged(int index) {
    emit memoryAlgorithmChanged(index);
}

void Dashboard::onTimeQuantumChanged(int quantum) {
    emit timeQuantumChanged(quantum);
}

void Dashboard::setupUI() {
    main_layout_ = new QVBoxLayout(this);
    main_layout_->setSpacing(10);
    main_layout_->setContentsMargins(10, 10, 10, 10);
    
    setupControlButtons();
    setupSpeedControl();
    setupAlgorithmControls();
    setupProcessControls();
    
    // Add stretch to push everything to the top
    main_layout_->addStretch();
    
    // Now all widgets exist — safe to set initial state
    setSimulationRunning(false);
}

void Dashboard::setupControlButtons() {
    qDebug() << "[DB] Creating control_group_";
    control_group_ = new QGroupBox("Simulation Control", this);
    QVBoxLayout* control_layout = new QVBoxLayout(control_group_);

    qDebug() << "[DB] Creating buttons";
    start_button_ = new QPushButton("Start", this);
    pause_button_ = new QPushButton("Pause", this);
    stop_button_  = new QPushButton("Stop", this);
    reset_button_ = new QPushButton("Reset", this);
    step_button_  = new QPushButton("Step", this);
    
    // Style buttons
    QString button_style = "QPushButton { padding: 8px; font-weight: bold; }";
    start_button_->setStyleSheet(button_style + "QPushButton { background-color: #4CAF50; color: white; }");
    pause_button_->setStyleSheet(button_style + "QPushButton { background-color: #FF9800; color: white; }");
    stop_button_->setStyleSheet(button_style + "QPushButton { background-color: #F44336; color: white; }");
    reset_button_->setStyleSheet(button_style + "QPushButton { background-color: #2196F3; color: white; }");
    step_button_->setStyleSheet(button_style + "QPushButton { background-color: #9C27B0; color: white; }");
    
    // Connect signals
    connect(start_button_, &QPushButton::clicked, this, &Dashboard::onStartSimulation);
    connect(pause_button_, &QPushButton::clicked, this, &Dashboard::onPauseSimulation);
    connect(stop_button_, &QPushButton::clicked, this, &Dashboard::onStopSimulation);
    connect(reset_button_, &QPushButton::clicked, this, &Dashboard::onResetSimulation);
    connect(step_button_, &QPushButton::clicked, this, &Dashboard::onStepSimulation);
    
    // Layout buttons in grid
    QGridLayout* button_grid = new QGridLayout();
    button_grid->addWidget(start_button_, 0, 0);
    button_grid->addWidget(pause_button_, 0, 1);
    button_grid->addWidget(stop_button_, 1, 0);
    button_grid->addWidget(reset_button_, 1, 1);
    button_grid->addWidget(step_button_, 2, 0, 1, 2);
    
    control_layout->addLayout(button_grid);
    
    // Time display
    time_label_ = new QLabel("Simulation Time:", this);
    time_display_ = new QLabel("00:00", this);
    time_display_->setStyleSheet("font-family: monospace; font-size: 14px; font-weight: bold; color: #333;");
    
    control_layout->addWidget(time_label_);
    control_layout->addWidget(time_display_);
    
    main_layout_->addWidget(control_group_);
    
    // NOTE: setSimulationRunning() is called AFTER all widgets are created in setupUI()
}

void Dashboard::setupSpeedControl() {
    QLabel* speed_title = new QLabel("Speed Control:", this);
    QVBoxLayout* ctrl_layout = qobject_cast<QVBoxLayout*>(control_group_->layout());
    if (ctrl_layout) ctrl_layout->addWidget(speed_title);

    speed_slider_ = new QSlider(Qt::Horizontal, this);
    speed_slider_->setRange(1, 10);
    speed_slider_->setValue(1);
    speed_slider_->setTickPosition(QSlider::TicksBelow);
    speed_slider_->setTickInterval(1);

    speed_label_ = new QLabel("Speed: 1 steps/sec", this);
    speed_label_->setStyleSheet("font-size: 12px;");

    connect(speed_slider_, &QSlider::valueChanged, this, &Dashboard::onSpeedChanged);

    if (ctrl_layout) {
        ctrl_layout->addWidget(speed_slider_);
        ctrl_layout->addWidget(speed_label_);
    }
}

void Dashboard::setupAlgorithmControls() {
    algorithm_group_ = new QGroupBox("Algorithm Selection", this);
    QVBoxLayout* algo_layout = new QVBoxLayout(algorithm_group_);
    
    // Scheduling algorithm
    QLabel* scheduler_label = new QLabel("CPU Scheduler:", this);
    scheduler_combo_ = new QComboBox(this);
    scheduler_combo_->addItems({"FCFS", "Round Robin", "Priority"});
    scheduler_combo_->setCurrentIndex(0);
    
    connect(scheduler_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Dashboard::onSchedulerChanged);
    
    algo_layout->addWidget(scheduler_label);
    algo_layout->addWidget(scheduler_combo_);
    
    // Time quantum (for Round Robin)
    quantum_label_ = new QLabel("Time Quantum:", this);
    time_quantum_spin_ = new QSpinBox(this);
    time_quantum_spin_->setRange(1, 20);
    time_quantum_spin_->setValue(4);
    time_quantum_spin_->setEnabled(false); // Initially disabled
    quantum_label_->setEnabled(false);
    
    connect(time_quantum_spin_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &Dashboard::onTimeQuantumChanged);
    
    algo_layout->addWidget(quantum_label_);
    algo_layout->addWidget(time_quantum_spin_);
    
    // Memory algorithm
    QLabel* memory_label = new QLabel("Memory Algorithm:", this);
    memory_algo_combo_ = new QComboBox(this);
    memory_algo_combo_->addItems({"FIFO", "LRU"});
    memory_algo_combo_->setCurrentIndex(1); // Default to LRU
    
    connect(memory_algo_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Dashboard::onMemoryAlgorithmChanged);
    
    algo_layout->addWidget(memory_label);
    algo_layout->addWidget(memory_algo_combo_);
    
    main_layout_->addWidget(algorithm_group_);
}

void Dashboard::setupProcessControls() {
    process_group_ = new QGroupBox("Add Process", this);
    QVBoxLayout* process_layout = new QVBoxLayout(process_group_);
    
    // Priority
    QLabel* priority_label = new QLabel("Priority (1-10):", this);
    priority_spin_ = new QSpinBox(this);
    priority_spin_->setRange(1, 10);
    priority_spin_->setValue(5);
    
    process_layout->addWidget(priority_label);
    process_layout->addWidget(priority_spin_);
    
    // Burst time
    QLabel* burst_label = new QLabel("Burst Time:", this);
    burst_time_spin_ = new QSpinBox(this);
    burst_time_spin_->setRange(1, 100);
    burst_time_spin_->setValue(10);
    
    process_layout->addWidget(burst_label);
    process_layout->addWidget(burst_time_spin_);
    
    // Arrival time
    QLabel* arrival_label = new QLabel("Arrival Time:", this);
    arrival_time_spin_ = new QSpinBox(this);
    arrival_time_spin_->setRange(0, 1000);
    arrival_time_spin_->setValue(0);
    
    process_layout->addWidget(arrival_label);
    process_layout->addWidget(arrival_time_spin_);
    
    // Add button
    add_process_button_ = new QPushButton("Add Process", this);
    add_process_button_->setStyleSheet("QPushButton { padding: 6px; background-color: #607D8B; color: white; font-weight: bold; }");
    
    connect(add_process_button_, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "Add Process", 
            QString("Process will be added with:\nPriority: %1\nBurst Time: %2\nArrival Time: %3")
            .arg(priority_spin_->value())
            .arg(burst_time_spin_->value())
            .arg(arrival_time_spin_->value()));
    });
    
    process_layout->addWidget(add_process_button_);
    
    main_layout_->addWidget(process_group_);
}

void Dashboard::updateTimeDisplay() {
    int minutes = current_time_ / 60;
    int seconds = current_time_ % 60;
    time_display_->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
}

// end of Dashboard.cpp