#include "MetricsWidget.h"
#include <iomanip>
#include <sstream>

MetricsWidget::MetricsWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void MetricsWidget::updateMetrics(const SystemMetrics& metrics) {
    // Timing
    avg_waiting_label_->setText(QString::number(metrics.average_waiting_time, 'f', 2));
    avg_turnaround_label_->setText(QString::number(metrics.average_turnaround_time, 'f', 2));
    avg_response_label_->setText(QString::number(metrics.average_response_time, 'f', 2));

    // CPU
    double cpu = std::min(100.0, metrics.cpu_utilization);
    cpu_util_label_->setText(QString::number(cpu, 'f', 1) + "%");
    cpu_util_bar_->setValue(static_cast<int>(cpu));
    throughput_label_->setText(QString::number(metrics.throughput, 'f', 3));

    // Processes
    total_processes_label_->setText(QString::number(metrics.total_processes));
    completed_processes_label_->setText(QString::number(metrics.completed_processes));
    current_time_label_->setText(QString::number(metrics.current_time));

    // Memory
    double mem = std::min(100.0, metrics.memory_utilization);
    mem_util_label_->setText(QString::number(mem, 'f', 1) + "%");
    mem_util_bar_->setValue(static_cast<int>(mem));
    page_faults_label_->setText(QString::number(metrics.page_faults));
    page_replacements_label_->setText(QString::number(metrics.page_replacements));
}

void MetricsWidget::resetMetrics() {
    avg_waiting_label_->setText("0.00");
    avg_turnaround_label_->setText("0.00");
    avg_response_label_->setText("0.00");
    cpu_util_label_->setText("0.0%");
    cpu_util_bar_->setValue(0);
    throughput_label_->setText("0.000");
    total_processes_label_->setText("0");
    completed_processes_label_->setText("0");
    current_time_label_->setText("0");
    mem_util_label_->setText("0.0%");
    mem_util_bar_->setValue(0);
    page_faults_label_->setText("0");
    page_replacements_label_->setText("0");
}

QLabel* MetricsWidget::makeValueLabel(const QString& initial) {
    auto* lbl = new QLabel(initial, this);
    lbl->setStyleSheet("font-size: 13px; font-weight: bold; color: #1565C0;");
    lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    return lbl;
}

void MetricsWidget::setupUI() {
    main_layout_ = new QVBoxLayout(this);
    main_layout_->setContentsMargins(8, 8, 8, 8);
    main_layout_->setSpacing(8);

    QString row_style = "QLabel { font-size: 11px; color: #333; }";

    // ---- Timing Group ----
    auto* timing_group = new QGroupBox("Timing Metrics", this);
    auto* tg_layout = new QGridLayout(timing_group);
    tg_layout->setColumnStretch(1, 1);

    auto addRow = [&](QGridLayout* gl, int row, const QString& name, QLabel*& val_label, const QString& unit = "") {
        auto* name_lbl = new QLabel(name, this);
        name_lbl->setStyleSheet(row_style);
        val_label = makeValueLabel("0.00");
        gl->addWidget(name_lbl, row, 0);
        gl->addWidget(val_label, row, 1);
        if (!unit.isEmpty()) {
            auto* unit_lbl = new QLabel(unit, this);
            unit_lbl->setStyleSheet("font-size: 10px; color: #777;");
            gl->addWidget(unit_lbl, row, 2);
        }
    };

    addRow(tg_layout, 0, "Avg Waiting Time:", avg_waiting_label_, "units");
    addRow(tg_layout, 1, "Avg Turnaround Time:", avg_turnaround_label_, "units");
    addRow(tg_layout, 2, "Avg Response Time:", avg_response_label_, "units");
    main_layout_->addWidget(timing_group);

    // ---- CPU Group ----
    auto* cpu_group = new QGroupBox("CPU Performance", this);
    auto* cg_layout = new QGridLayout(cpu_group);
    cg_layout->setColumnStretch(1, 1);

    auto* cpu_name = new QLabel("CPU Utilization:", this);
    cpu_name->setStyleSheet(row_style);
    cpu_util_label_ = makeValueLabel("0.0%");
    cpu_util_bar_ = new QProgressBar(this);
    cpu_util_bar_->setRange(0, 100);
    cpu_util_bar_->setValue(0);
    cpu_util_bar_->setTextVisible(false);
    cpu_util_bar_->setStyleSheet(
        "QProgressBar { border: 1px solid #ccc; border-radius: 4px; background: #eee; height: 10px; }"
        "QProgressBar::chunk { background: #4CAF50; border-radius: 4px; }");

    cg_layout->addWidget(cpu_name, 0, 0);
    cg_layout->addWidget(cpu_util_label_, 0, 1);
    cg_layout->addWidget(cpu_util_bar_, 1, 0, 1, 2);

    auto* tp_name = new QLabel("Throughput:", this);
    tp_name->setStyleSheet(row_style);
    throughput_label_ = makeValueLabel("0.000");
    auto* tp_unit = new QLabel("proc/unit", this);
    tp_unit->setStyleSheet("font-size: 10px; color: #777;");
    cg_layout->addWidget(tp_name, 2, 0);
    cg_layout->addWidget(throughput_label_, 2, 1);
    cg_layout->addWidget(tp_unit, 2, 2);
    main_layout_->addWidget(cpu_group);

    // ---- Process Group ----
    auto* proc_group = new QGroupBox("Process Statistics", this);
    auto* pg_layout = new QGridLayout(proc_group);
    pg_layout->setColumnStretch(1, 1);

    auto addProcRow = [&](int row, const QString& name, QLabel*& lbl) {
        auto* n = new QLabel(name, this);
        n->setStyleSheet(row_style);
        lbl = makeValueLabel("0");
        pg_layout->addWidget(n, row, 0);
        pg_layout->addWidget(lbl, row, 1);
    };

    addProcRow(0, "Total Processes:", total_processes_label_);
    addProcRow(1, "Completed:", completed_processes_label_);
    addProcRow(2, "Current Time:", current_time_label_);
    main_layout_->addWidget(proc_group);

    // ---- Memory Group ----
    auto* mem_group = new QGroupBox("Memory Statistics", this);
    auto* mg_layout = new QGridLayout(mem_group);
    mg_layout->setColumnStretch(1, 1);

    auto* mem_name = new QLabel("Memory Utilization:", this);
    mem_name->setStyleSheet(row_style);
    mem_util_label_ = makeValueLabel("0.0%");
    mem_util_bar_ = new QProgressBar(this);
    mem_util_bar_->setRange(0, 100);
    mem_util_bar_->setValue(0);
    mem_util_bar_->setTextVisible(false);
    mem_util_bar_->setStyleSheet(
        "QProgressBar { border: 1px solid #ccc; border-radius: 4px; background: #eee; height: 10px; }"
        "QProgressBar::chunk { background: #2196F3; border-radius: 4px; }");

    mg_layout->addWidget(mem_name, 0, 0);
    mg_layout->addWidget(mem_util_label_, 0, 1);
    mg_layout->addWidget(mem_util_bar_, 1, 0, 1, 2);

    auto* pf_name = new QLabel("Page Faults:", this);
    pf_name->setStyleSheet(row_style);
    page_faults_label_ = makeValueLabel("0");
    mg_layout->addWidget(pf_name, 2, 0);
    mg_layout->addWidget(page_faults_label_, 2, 1);

    auto* pr_name = new QLabel("Page Replacements:", this);
    pr_name->setStyleSheet(row_style);
    page_replacements_label_ = makeValueLabel("0");
    mg_layout->addWidget(pr_name, 3, 0);
    mg_layout->addWidget(page_replacements_label_, 3, 1);
    main_layout_->addWidget(mem_group);

    // Export button
    export_button_ = new QPushButton("Export Metrics (CSV)", this);
    export_button_->setStyleSheet(
        "QPushButton { padding: 6px; background-color: #607D8B; color: white; font-weight: bold; border-radius: 4px; }"
        "QPushButton:hover { background-color: #546E7A; }");
    connect(export_button_, &QPushButton::clicked, this, &MetricsWidget::exportRequested);
    main_layout_->addWidget(export_button_);

    main_layout_->addStretch();
}

#include "MetricsWidget.moc"