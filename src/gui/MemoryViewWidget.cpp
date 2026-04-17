#include "MemoryViewWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QScrollArea>
#include <algorithm>

const std::vector<QColor> MemoryViewWidget::process_colors_ = {
    QColor(255, 99, 132),  QColor(54, 162, 235),  QColor(255, 205, 86),
    QColor(75, 192, 192),  QColor(153, 102, 255), QColor(255, 159, 64),
    QColor(199, 199, 199), QColor(83, 102, 255),  QColor(255, 99, 255),
    QColor(99, 255, 132)
};

// ============================================================================
// FrameWidget Implementation
// ============================================================================

MemoryViewWidget::FrameWidget::FrameWidget(int frame_id, QWidget* parent)
    : QFrame(parent), frame_id_(frame_id),
      frame_data_(frame_id), highlighted_(false) {
    setFixedSize(70, 55);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(1);
    setCursor(Qt::PointingHandCursor);
    
    label_ = new QLabel(this);
    label_->setAlignment(Qt::AlignCenter);
    label_->setGeometry(2, 2, 66, 51);
    label_->setWordWrap(true);
    label_->setStyleSheet("font-size: 9px;");
}

void MemoryViewWidget::FrameWidget::setFrameData(const PageFrame& frame) {
    frame_data_ = frame;
    update();
}

void MemoryViewWidget::FrameWidget::setHighlighted(bool highlighted) {
    highlighted_ = highlighted;
    update();
}

void MemoryViewWidget::FrameWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(frame_id_);
    }
    QFrame::mousePressEvent(event);
}

void MemoryViewWidget::FrameWidget::paintEvent(QPaintEvent* event) {
    QFrame::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect r = rect().adjusted(1, 1, -1, -1);

    if (highlighted_) {
        painter.fillRect(r, QColor(255, 50, 50, 180));
        setToolTip(QString("Frame %1 [REPLACED]").arg(frame_id_));
    } else if (frame_data_.is_allocated) {
        QColor c = MemoryViewWidget::process_colors_[frame_data_.process_id % MemoryViewWidget::process_colors_.size()];
        painter.fillRect(r, c.lighter(140));
        setToolTip(QString("Frame %1\nPID: %2\nPage: %3")
                   .arg(frame_id_).arg(frame_data_.process_id).arg(frame_data_.page_number));
    } else {
        painter.fillRect(r, QColor(240, 240, 240));
        setToolTip(QString("Frame %1 [FREE]").arg(frame_id_));
    }

    // Draw text
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 8));
    QString text = QString("F%1\n").arg(frame_id_);
    if (frame_data_.is_allocated) {
        text += QString("P%1\nPg%2").arg(frame_data_.process_id).arg(frame_data_.page_number);
    } else {
        text += "FREE";
    }
    painter.drawText(r, Qt::AlignCenter, text);
}

// ============================================================================
// MemoryViewWidget Implementation
// ============================================================================

MemoryViewWidget::MemoryViewWidget(QWidget* parent)
    : QWidget(parent), selected_process_(-1), highlighted_frame_(-1), frames_per_row_(8) {
    setupUI();
}

void MemoryViewWidget::updateMemoryFrames(const std::vector<PageFrame>& frames) {
    current_frames_ = frames;

    // Add new frame widgets if needed
    while (static_cast<int>(frame_widgets_.size()) < static_cast<int>(frames.size())) {
        int id = static_cast<int>(frame_widgets_.size());
        auto* fw = new FrameWidget(id, this);
        connect(fw, &FrameWidget::clicked, this, &MemoryViewWidget::frameClicked);
        frame_widgets_.push_back(fw);
        int row = id / frames_per_row_;
        int col = id % frames_per_row_;
        memory_grid_->addWidget(fw, row, col);
    }

    // Update each frame widget
    for (int i = 0; i < static_cast<int>(frames.size()); ++i) {
        frame_widgets_[i]->setFrameData(frames[i]);
        frame_widgets_[i]->setHighlighted(i == highlighted_frame_);
    }

    // Update utilization label
    int allocated = 0;
    for (const auto& f : frames) {
        if (f.is_allocated) allocated++;
    }
    double util = frames.empty() ? 0.0 : (100.0 * allocated / frames.size());
    utilization_label_->setText(QString("Utilization: %1% (%2/%3 frames)")
                                .arg(util, 0, 'f', 1)
                                .arg(allocated)
                                .arg(frames.size()));
}

void MemoryViewWidget::updatePageTable(int process_id, const std::vector<PageTableEntry>& page_table) {
    process_page_tables_[process_id] = page_table;
    updateProcessSelector();

    if (process_id == selected_process_) {
        // Refresh page table display
        onProcessSelectionChanged(process_selector_->currentIndex());
    }
}

void MemoryViewWidget::setSelectedProcess(int process_id) {
    selected_process_ = process_id;
    // Find in combo
    for (int i = 0; i < process_selector_->count(); ++i) {
        if (process_selector_->itemData(i).toInt() == process_id) {
            process_selector_->setCurrentIndex(i);
            break;
        }
    }
}

void MemoryViewWidget::highlightFrame(int frame_id) {
    highlighted_frame_ = frame_id;
    for (auto* fw : frame_widgets_) {
        fw->setHighlighted(fw->getFrameId() == frame_id);
    }
}

void MemoryViewWidget::clearMemory() {
    current_frames_.clear();
    process_page_tables_.clear();
    highlighted_frame_ = -1;
    selected_process_ = -1;

    for (auto* fw : frame_widgets_) {
        PageFrame empty(fw->getFrameId());
        fw->setFrameData(empty);
        fw->setHighlighted(false);
    }

    page_table_->setRowCount(0);
    process_selector_->clear();
    utilization_label_->setText("Utilization: 0%");
}

void MemoryViewWidget::setMemoryConfiguration(const MemoryConfiguration& config) {
    memory_config_ = config;
    memory_info_label_->setText(
        QString("Total: %1 bytes | Page Size: %2 bytes | Frames: %3")
        .arg(config.total_memory_size)
        .arg(config.page_size)
        .arg(config.num_frames));
}

void MemoryViewWidget::onProcessSelectionChanged(int index) {
    if (index < 0) return;
    int pid = process_selector_->itemData(index).toInt();
    selected_process_ = pid;
    emit selectedProcessChanged(pid);

    // Populate page table
    page_table_->setRowCount(0);
    auto it = process_page_tables_.find(pid);
    if (it == process_page_tables_.end()) return;

    for (const auto& entry : it->second) {
        int row = page_table_->rowCount();
        page_table_->insertRow(row);

        auto* vp = new QTableWidgetItem(QString::number(entry.virtual_page));
        auto* pf = new QTableWidgetItem(entry.is_valid ? QString::number(entry.physical_frame) : "N/A");
        auto* valid = new QTableWidgetItem(entry.is_valid ? "Yes" : "No");
        auto* dirty = new QTableWidgetItem(entry.is_dirty ? "Yes" : "No");

        vp->setTextAlignment(Qt::AlignCenter);
        pf->setTextAlignment(Qt::AlignCenter);
        valid->setTextAlignment(Qt::AlignCenter);
        dirty->setTextAlignment(Qt::AlignCenter);

        if (!entry.is_valid) {
            pf->setForeground(Qt::red);
        }

        page_table_->setItem(row, 0, vp);
        page_table_->setItem(row, 1, pf);
        page_table_->setItem(row, 2, valid);
        page_table_->setItem(row, 3, dirty);
    }
}

void MemoryViewWidget::setupUI() {
    main_layout_ = new QVBoxLayout(this);
    main_layout_->setContentsMargins(5, 5, 5, 5);
    main_layout_->setSpacing(8);

    // Info labels
    memory_info_label_ = new QLabel("Memory not configured", this);
    memory_info_label_->setStyleSheet("font-size: 11px; color: #555;");
    utilization_label_ = new QLabel("Utilization: 0%", this);
    utilization_label_->setStyleSheet("font-size: 11px; font-weight: bold; color: #333;");

    main_layout_->addWidget(memory_info_label_);
    main_layout_->addWidget(utilization_label_);

    top_layout_ = new QHBoxLayout();
    setupMemoryGrid();
    setupPageTableView();
    main_layout_->addLayout(top_layout_);
}

void MemoryViewWidget::setupMemoryGrid() {
    memory_group_ = new QGroupBox("Memory Frames", this);
    memory_group_->setMinimumWidth(400);

    QVBoxLayout* vl = new QVBoxLayout(memory_group_);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);

    QWidget* grid_container = new QWidget();
    memory_grid_ = new QGridLayout(grid_container);
    memory_grid_->setSpacing(4);
    memory_grid_->setContentsMargins(4, 4, 4, 4);

    scroll->setWidget(grid_container);
    vl->addWidget(scroll);

    top_layout_->addWidget(memory_group_);
}

void MemoryViewWidget::setupPageTableView() {
    page_table_group_ = new QGroupBox("Page Table", this);
    page_table_layout_ = new QVBoxLayout(page_table_group_);

    // Process selector
    QHBoxLayout* selector_layout = new QHBoxLayout();
    QLabel* selector_label = new QLabel("Process:", this);
    process_selector_ = new QComboBox(this);
    process_selector_->setMinimumWidth(100);
    connect(process_selector_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MemoryViewWidget::onProcessSelectionChanged);

    selector_layout->addWidget(selector_label);
    selector_layout->addWidget(process_selector_);
    selector_layout->addStretch();
    page_table_layout_->addLayout(selector_layout);

    // Page table widget
    page_table_ = new QTableWidget(0, 4, this);
    page_table_->setHorizontalHeaderLabels({"Virtual Page", "Physical Frame", "Valid", "Dirty"});
    page_table_->horizontalHeader()->setStretchLastSection(true);
    page_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    page_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    page_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    page_table_->setAlternatingRowColors(true);
    page_table_->setStyleSheet(
        "QHeaderView::section { background-color: #2196F3; color: white; padding: 4px; font-weight: bold; }"
    );

    page_table_layout_->addWidget(page_table_);
    top_layout_->addWidget(page_table_group_);
}

void MemoryViewWidget::updateProcessSelector() {
    int current_pid = selected_process_;
    process_selector_->blockSignals(true);
    process_selector_->clear();

    for (const auto& pair : process_page_tables_) {
        process_selector_->addItem(QString("PID %1").arg(pair.first), pair.first);
    }

    // Restore selection
    for (int i = 0; i < process_selector_->count(); ++i) {
        if (process_selector_->itemData(i).toInt() == current_pid) {
            process_selector_->setCurrentIndex(i);
            break;
        }
    }
    process_selector_->blockSignals(false);
}

QColor MemoryViewWidget::getProcessColor(int process_id) const {
    return process_colors_[process_id % process_colors_.size()];
}

#include "MemoryViewWidget.moc"