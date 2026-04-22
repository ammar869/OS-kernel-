#pragma once

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

/**
 * @brief Dashboard widget for simulation controls
 * 
 * The Dashboard provides the main control interface for the simulation,
 * including start/stop controls, speed adjustment, and algorithm selection.
 */
class Dashboard : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit Dashboard(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~Dashboard() = default;
    
    /**
     * @brief Set simulation time display
     * @param time Current simulation time
     */
    void setSimulationTime(int time);
    
    /**
     * @brief Set simulation speed
     * @param speed Speed in steps per second
     */
    void setSimulationSpeed(int speed);
    
    /**
     * @brief Get current simulation speed
     * @return Speed in steps per second
     */
    int getSimulationSpeed() const;
    
    /**
     * @brief Set scheduling algorithm
     * @param algorithm Algorithm index
     */
    void setSchedulingAlgorithm(int algorithm);
    
    /**
     * @brief Get selected scheduling algorithm
     * @return Algorithm index
     */
    int getSchedulingAlgorithm() const;
    
    /**
     * @brief Set memory replacement algorithm
     * @param algorithm Algorithm index
     */
    void setMemoryAlgorithm(int algorithm);
    
    /**
     * @brief Get selected memory algorithm
     * @return Algorithm index
     */
    int getMemoryAlgorithm() const;
    
    /**
     * @brief Set time quantum for Round Robin
     * @param quantum Time quantum value
     */
    void setTimeQuantum(int quantum);
    
    /**
     * @brief Get time quantum
     * @return Time quantum value
     */
    int getTimeQuantum() const;
    
    /**
     * @brief Enable/disable controls based on simulation state
     * @param running Whether simulation is running
     */
    void setSimulationRunning(bool running);

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

signals:
    void simulationStartRequested();
    void simulationPaused();
    void simulationStopped();
    void simulationReset();
    void stepRequested();
    void speedChanged(int steps_per_second);
    void schedulerChanged(int algorithm);
    void memoryAlgorithmChanged(int algorithm);
    void timeQuantumChanged(int quantum);
    void processAddRequested(int priority, int burst_time, int arrival_time);

private:
    void setupUI();
    void setupControlButtons();
    void setupSpeedControl();
    void setupAlgorithmControls();
    void setupProcessControls();
    void updateTimeDisplay();
    
    // Control buttons
    QPushButton* start_button_;
    QPushButton* pause_button_;
    QPushButton* stop_button_;
    QPushButton* reset_button_;
    QPushButton* step_button_;
    
    // Speed control
    QSlider* speed_slider_;
    QLabel* speed_label_;
    
    // Time display
    QLabel* time_display_;
    QLabel* time_label_;
    
    // Algorithm selection
    QComboBox* scheduler_combo_;
    QComboBox* memory_algo_combo_;
    QSpinBox* time_quantum_spin_;
    QLabel* quantum_label_;
    
    // Process creation controls
    QSpinBox* priority_spin_;
    QSpinBox* burst_time_spin_;
    QSpinBox* arrival_time_spin_;
    QPushButton* add_process_button_;
    
    // Layout containers
    QVBoxLayout* main_layout_;
    QGroupBox* control_group_;
    QGroupBox* algorithm_group_;
    QGroupBox* process_group_;
    
    // Current values
    int current_time_;
    bool simulation_running_;
};