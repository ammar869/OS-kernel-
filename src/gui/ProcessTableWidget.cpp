#include "ProcessTableWidget.h"
#include <QHeaderView>
#include <QBrush>
#include <QColor>
#include <algorithm>

ProcessTableWidget::ProcessTableWidget(QWidget* parent)
    : QTableWidget(parent), highlighted_pid_(-1), auto_refresh_enabled_(false) {
    
    setupColumns();
    setupStyling();
    
    // Setup auto-refresh timer
    refresh_timer_ = new QTimer(this);
    connect(refresh_timer_, &QTimer::timeout, this, &ProcessTableWidget::refreshTable);
    
    // Connect selection signal
    connect(this, &QTableWidget::cellClicked, this, &ProcessTableWidget::onCellClicked);
}

void ProcessTableWidget::updateProcesses(const std::vector<ProcessControlBlock*>& processes) {
    // Store current processes
    current_processes_.clear();
    for (const auto* pcb : processes) {
        if (pcb) {
            current_processes_.push_back(*pcb);
        }
    }
    
    // Clear existing rows
    setRowCount(0);
    
    // Add processes sorted by PID
    std::vector<ProcessControlBlock> sorted_processes = current_processes_;
    std::sort(sorted_processes.begin(), sorted_processes.end(),
              [](const ProcessControlBlock& a, const ProcessControlBlock& b) {
                  return a.pid < b.pid;
              });
    
    for (const auto& process : sorted_processes) {
        addProcessRow(&process);
    }
    
    // Restore highlighting
    if (highlighted_pid_ != -1) {
        highlightRunningProcess(highlighted_pid_);
    }
}

void ProcessTableWidget::highlightRunningProcess(int pid) {
    highlighted_pid_ = pid;
    
    // Remove previous highlighting
    for (int row = 0; row < rowCount(); ++row) {
        for (int col = 0; col < columnCount(); ++col) {
            QTableWidgetItem* item = this->item(row, col);
            if (item) {
                item->setBackground(QBrush(Qt::white));
                item->setData(Qt::FontRole, QFont());
            }
        }
    }
    
    if (pid == -1) {
        return; // Clear highlighting only
    }
    
    // Find and highlight the running process
    int row = findProcessRow(pid);
    if (row != -1) {
        QColor highlight_color(255, 255, 0, 100); // Light yellow
        QFont bold_font;
        bold_font.setBold(true);
        
        for (int col = 0; col < columnCount(); ++col) {
            QTableWidgetItem* item = this->item(row, col);
            if (item) {
                item->setBackground(QBrush(highlight_color));
                item->setFont(bold_font);
            }
        }
    }
}

void ProcessTableWidget::clearProcesses() {
    current_processes_.clear();
    highlighted_pid_ = -1;
    setRowCount(0);
}

void ProcessTableWidget::setRefreshInterval(int interval_ms) {
    if (interval_ms > 0) {
        refresh_timer_->setInterval(interval_ms);
    }
}

void ProcessTableWidget::setAutoRefresh(bool enabled) {
    auto_refresh_enabled_ = enabled;
    
    if (enabled) {
        refresh_timer_->start();
    } else {
        refresh_timer_->stop();
    }
}

void ProcessTableWidget::refreshTable() {
    if (!auto_refresh_enabled_) {
        return;
    }
    
    // This would typically be connected to get fresh data from the simulation
    // For now, just update the display with current data
    std::vector<ProcessControlBlock*> process_ptrs;
    for (auto& process : current_processes_) {
        process_ptrs.push_back(&process);
    }
    updateProcesses(process_ptrs);
}

void ProcessTableWidget::onCellClicked(int row, int column) {
    Q_UNUSED(column)
    
    if (row >= 0 && row < rowCount()) {
        QTableWidgetItem* pid_item = item(row, PID);
        if (pid_item) {
            int pid = pid_item->text().toInt();
            emit processSelected(pid);
        }
    }
}

void ProcessTableWidget::setupColumns() {
    setColumnCount(COLUMN_COUNT);
    
    // Set column headers
    QStringList headers;
    headers << "PID" << "State" << "Priority" << "Burst Time" << "Remaining" << "Arrival";
    setHorizontalHeaderLabels(headers);
    
    // Set column widths
    horizontalHeader()->setStretchLastSection(false);
    setColumnWidth(PID, 60);
    setColumnWidth(STATE, 80);
    setColumnWidth(PRIORITY, 70);
    setColumnWidth(BURST_TIME, 80);
    setColumnWidth(REMAINING_TIME, 80);
    setColumnWidth(ARRIVAL_TIME, 70);
    
    // Make columns resizable
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    
    // Set row selection behavior
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void ProcessTableWidget::setupStyling() {
    // Set alternating row colors
    setAlternatingRowColors(true);
    
    // Set grid style
    setShowGrid(true);
    setGridStyle(Qt::SolidLine);
    
    // Set font
    QFont table_font;
    table_font.setFamily("Consolas, Monaco, monospace");
    table_font.setPointSize(10);
    setFont(table_font);
    
    // Set header style
    horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    padding: 4px;"
        "    border: 1px solid #45a049;"
        "    font-weight: bold;"
        "}"
    );
    
    // Set table style
    setStyleSheet(
        "QTableWidget {"
        "    background-color: white;"
        "    alternate-background-color: #f5f5f5;"
        "    selection-background-color: #e3f2fd;"
        "    gridline-color: #ddd;"
        "}"
        "QTableWidget::item {"
        "    padding: 4px;"
        "    border: none;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #2196f3;"
        "    color: white;"
        "}"
    );
}

void ProcessTableWidget::addProcessRow(const ProcessControlBlock* process) {
    if (!process) return;
    
    int row = rowCount();
    insertRow(row);
    
    // PID
    QTableWidgetItem* pid_item = new QTableWidgetItem(QString::number(process->pid));
    pid_item->setTextAlignment(Qt::AlignCenter);
    pid_item->setFlags(pid_item->flags() & ~Qt::ItemIsEditable);
    setItem(row, PID, pid_item);
    
    // State
    QTableWidgetItem* state_item = new QTableWidgetItem(getStateText(process->state));
    state_item->setTextAlignment(Qt::AlignCenter);
    state_item->setFlags(state_item->flags() & ~Qt::ItemIsEditable);
    state_item->setForeground(QBrush(QColor(getStateColor(process->state))));
    setItem(row, STATE, state_item);
    
    // Priority
    QTableWidgetItem* priority_item = new QTableWidgetItem(QString::number(process->priority));
    priority_item->setTextAlignment(Qt::AlignCenter);
    priority_item->setFlags(priority_item->flags() & ~Qt::ItemIsEditable);
    setItem(row, PRIORITY, priority_item);
    
    // Burst Time
    QTableWidgetItem* burst_item = new QTableWidgetItem(QString::number(process->burst_time));
    burst_item->setTextAlignment(Qt::AlignCenter);
    burst_item->setFlags(burst_item->flags() & ~Qt::ItemIsEditable);
    setItem(row, BURST_TIME, burst_item);
    
    // Remaining Time
    QTableWidgetItem* remaining_item = new QTableWidgetItem(QString::number(process->remaining_time));
    remaining_item->setTextAlignment(Qt::AlignCenter);
    remaining_item->setFlags(remaining_item->flags() & ~Qt::ItemIsEditable);
    setItem(row, REMAINING_TIME, remaining_item);
    
    // Arrival Time
    QTableWidgetItem* arrival_item = new QTableWidgetItem(QString::number(process->arrival_time));
    arrival_item->setTextAlignment(Qt::AlignCenter);
    arrival_item->setFlags(arrival_item->flags() & ~Qt::ItemIsEditable);
    setItem(row, ARRIVAL_TIME, arrival_item);
}

void ProcessTableWidget::updateProcessRow(int row, const ProcessControlBlock* process) {
    if (!process || row < 0 || row >= rowCount()) return;
    
    // Update state
    QTableWidgetItem* state_item = item(row, STATE);
    if (state_item) {
        state_item->setText(getStateText(process->state));
        state_item->setForeground(QBrush(QColor(getStateColor(process->state))));
    }
    
    // Update remaining time
    QTableWidgetItem* remaining_item = item(row, REMAINING_TIME);
    if (remaining_item) {
        remaining_item->setText(QString::number(process->remaining_time));
    }
}

int ProcessTableWidget::findProcessRow(int pid) const {
    for (int row = 0; row < rowCount(); ++row) {
        QTableWidgetItem* pid_item = item(row, PID);
        if (pid_item && pid_item->text().toInt() == pid) {
            return row;
        }
    }
    return -1;
}

void ProcessTableWidget::removeProcessRow(int pid) {
    int row = findProcessRow(pid);
    if (row != -1) {
        removeRow(row);
    }
}

QString ProcessTableWidget::getStateColor(ProcessState state) const {
    switch (state) {
        case ProcessState::NEW:
            return "#9E9E9E";      // Gray
        case ProcessState::READY:
            return "#2196F3";      // Blue
        case ProcessState::RUNNING:
            return "#4CAF50";      // Green
        case ProcessState::WAITING:
            return "#FF9800";      // Orange
        case ProcessState::TERMINATED:
            return "#F44336";      // Red
        default:
            return "#000000";      // Black
    }
}

QString ProcessTableWidget::getStateText(ProcessState state) const {
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

#include "ProcessTableWidget.moc"