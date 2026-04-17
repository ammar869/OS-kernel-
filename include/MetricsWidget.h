#pragma once

#include "SimulationConfig.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QPushButton>

/**
 * @brief Widget for displaying real-time simulation performance metrics
 */
class MetricsWidget : public QWidget {
    Q_OBJECT

public:
    explicit MetricsWidget(QWidget* parent = nullptr);
    ~MetricsWidget() = default;

    /**
     * @brief Update all metrics from a SystemMetrics snapshot
     * @param metrics Current system metrics
     */
    void updateMetrics(const SystemMetrics& metrics);

    /**
     * @brief Reset all displayed metrics to zero
     */
    void resetMetrics();

signals:
    void exportRequested();

private:
    void setupUI();
    QLabel* makeValueLabel(const QString& initial = "0.00");

    // Timing metrics
    QLabel* avg_waiting_label_;
    QLabel* avg_turnaround_label_;
    QLabel* avg_response_label_;

    // CPU metrics
    QLabel* cpu_util_label_;
    QProgressBar* cpu_util_bar_;
    QLabel* throughput_label_;

    // Process counts
    QLabel* total_processes_label_;
    QLabel* completed_processes_label_;
    QLabel* current_time_label_;

    // Memory metrics
    QLabel* mem_util_label_;
    QProgressBar* mem_util_bar_;
    QLabel* page_faults_label_;
    QLabel* page_replacements_label_;

    // Export button
    QPushButton* export_button_;

    QVBoxLayout* main_layout_;
};