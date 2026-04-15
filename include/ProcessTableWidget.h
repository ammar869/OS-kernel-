#pragma once

#include "ProcessState.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <vector>

/**
 * @brief Table widget for displaying process information
 * 
 * The ProcessTableWidget displays real-time process information including
 * PID, state, priority, burst time, and remaining time in a tabular format.
 */
class ProcessTableWidget : public QTableWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit ProcessTableWidget(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~ProcessTableWidget() = default;
    
    /**
     * @brief Update the table with current process information
     * @param processes Vector of process control blocks
     */
    void updateProcesses(const std::vector<ProcessControlBlock*>& processes);
    
    /**
     * @brief Highlight the currently running process
     * @param pid Process ID to highlight, -1 to clear highlight
     */
    void highlightRunningProcess(int pid);
    
    /**
     * @brief Clear all process data
     */
    void clearProcesses();
    
    /**
     * @brief Set auto-refresh interval
     * @param interval_ms Refresh interval in milliseconds
     */
    void setRefreshInterval(int interval_ms);
    
    /**
     * @brief Enable/disable auto-refresh
     * @param enabled Whether auto-refresh is enabled
     */
    void setAutoRefresh(bool enabled);

public slots:
    /**
     * @brief Refresh the table display
     */
    void refreshTable();

signals:
    /**
     * @brief Emitted when a process is selected
     * @param pid Selected process ID
     */
    void processSelected(int pid);

private slots:
    void onCellClicked(int row, int column);

private:
    enum Columns {
        PID = 0,
        STATE = 1,
        PRIORITY = 2,
        BURST_TIME = 3,
        REMAINING_TIME = 4,
        ARRIVAL_TIME = 5,
        COLUMN_COUNT = 6
    };
    
    void setupColumns();
    void setupStyling();
    void addProcessRow(const ProcessControlBlock* process);
    void updateProcessRow(int row, const ProcessControlBlock* process);
    int findProcessRow(int pid) const;
    void removeProcessRow(int pid);
    QString getStateColor(ProcessState state) const;
    QString getStateText(ProcessState state) const;
    
    // Current process data
    std::vector<ProcessControlBlock> current_processes_;
    int highlighted_pid_;
    
    // Auto-refresh
    QTimer* refresh_timer_;
    bool auto_refresh_enabled_;
};