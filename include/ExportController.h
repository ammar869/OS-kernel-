#pragma once

#include "SimulationConfig.h"
#include "GanttChartWidget.h"
#include <QString>
#include <string>

/**
 * @brief Handles exporting simulation results to files
 */
class ExportController {
public:
    ExportController() = default;
    ~ExportController() = default;

    /**
     * @brief Export Gantt chart as PNG image
     * @param widget Gantt chart widget to capture
     * @param filename Output file path
     * @return true on success
     */
    bool exportGanttChart(GanttChartWidget* widget, const QString& filename);

    /**
     * @brief Export metrics as CSV
     * @param metrics System metrics snapshot
     * @param filename Output file path
     * @return true on success
     */
    bool exportMetricsCSV(const SystemMetrics& metrics, const QString& filename);

    /**
     * @brief Export simulation log as text
     * @param log_content Log text
     * @param filename Output file path
     * @return true on success
     */
    bool exportSimulationLog(const std::string& log_content, const QString& filename);

    /**
     * @brief Get last error message
     */
    const QString& getLastError() const { return last_error_; }

private:
    QString last_error_;
    void setError(const QString& msg) { last_error_ = msg; }
};