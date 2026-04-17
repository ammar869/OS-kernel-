#include "ExportController.h"
#include <QPixmap>
#include <QDateTime>
#include <fstream>
#include <sstream>
#include <iomanip>

bool ExportController::exportGanttChart(GanttChartWidget* widget, const QString& filename) {
    if (!widget) {
        setError("Gantt chart widget is null");
        return false;
    }

    try {
        QPixmap pixmap = widget->grab();
        if (pixmap.isNull()) {
            setError("Failed to capture Gantt chart");
            return false;
        }

        if (!pixmap.save(filename)) {
            setError("Failed to save image to: " + filename);
            return false;
        }
        return true;
    }
    catch (const std::exception& e) {
        setError(QString("Exception: ") + e.what());
        return false;
    }
}

bool ExportController::exportMetricsCSV(const SystemMetrics& metrics, const QString& filename) {
    try {
        std::ofstream file(filename.toStdString());
        if (!file.is_open()) {
            setError("Cannot open file: " + filename);
            return false;
        }

        // Timestamp header
        file << "# Mini OS Kernel Simulator - Metrics Export\n";
        file << "# Exported: " << QDateTime::currentDateTime().toString(Qt::ISODate).toStdString() << "\n";
        file << "\n";

        // CSV header
        file << "Metric,Value\n";
        file << std::fixed << std::setprecision(2);
        file << "Simulation Time," << metrics.current_time << "\n";
        file << "Total Processes," << metrics.total_processes << "\n";
        file << "Completed Processes," << metrics.completed_processes << "\n";
        file << "Average Waiting Time," << metrics.average_waiting_time << "\n";
        file << "Average Turnaround Time," << metrics.average_turnaround_time << "\n";
        file << "Average Response Time," << metrics.average_response_time << "\n";
        file << "CPU Utilization (%)," << metrics.cpu_utilization << "\n";
        file << "Throughput (proc/unit)," << metrics.throughput << "\n";
        file << "Memory Utilization (%)," << metrics.memory_utilization << "\n";
        file << "Page Faults," << metrics.page_faults << "\n";
        file << "Page Replacements," << metrics.page_replacements << "\n";

        file.close();
        return true;
    }
    catch (const std::exception& e) {
        setError(QString("Exception: ") + e.what());
        return false;
    }
}

bool ExportController::exportSimulationLog(const std::string& log_content, const QString& filename) {
    try {
        std::ofstream file(filename.toStdString());
        if (!file.is_open()) {
            setError("Cannot open file: " + filename);
            return false;
        }

        file << "# Mini OS Kernel Simulator - Simulation Log\n";
        file << "# Exported: " << QDateTime::currentDateTime().toString(Qt::ISODate).toStdString() << "\n\n";
        file << log_content;
        file.close();
        return true;
    }
    catch (const std::exception& e) {
        setError(QString("Exception: ") + e.what());
        return false;
    }
}