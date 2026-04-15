#include "GanttChartWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QApplication>
#include <algorithm>

// Static color palette for processes
const std::vector<QColor> GanttChartWidget::process_colors_ = {
    QColor(255, 99, 132),   // Red
    QColor(54, 162, 235),   // Blue
    QColor(255, 205, 86),   // Yellow
    QColor(75, 192, 192),   // Teal
    QColor(153, 102, 255),  // Purple
    QColor(255, 159, 64),   // Orange
    QColor(199, 199, 199),  // Gray
    QColor(83, 102, 255),   // Indigo
    QColor(255, 99, 255),   // Pink
    QColor(99, 255, 132)    // Green
};

GanttChartWidget::GanttChartWidget(QWidget* parent)
    : QWidget(parent), max_time_(100), zoom_level_(1.0), time_scale_(20),
      row_height_(40), axis_height_(30), margin_(10) {
    setupUI();
}

void GanttChartWidget::addTimeBlock(int process_id, int start_time, int end_time, const QString& process_name) {
    if (start_time >= end_time) return;
    
    // Ensure process exists in our tracking
    ensureProcessExists(process_id);
    
    // Store process name if provided
    if (!process_name.isEmpty()) {
        process_names_[process_id] = process_name;
    }
    
    // Add the time block
    time_blocks_.emplace_back(process_id, start_time, end_time, process_name);
    
    // Update max time if necessary
    if (end_time > max_time_) {
        setMaxTime(end_time + 10); // Add some padding
    }
    
    // Update display
    updateScrollArea();
    update();
}

void GanttChartWidget::clearChart() {
    time_blocks_.clear();
    process_names_.clear();
    process_order_.clear();
    max_time_ = 100;
    updateScrollArea();
    update();
}

void GanttChartWidget::setMaxTime(int max_time) {
    if (max_time > 0) {
        max_time_ = max_time;
        updateScrollArea();
        update();
    }
}

void GanttChartWidget::setZoomLevel(double zoom_level) {
    if (zoom_level > 0.1 && zoom_level <= 5.0) {
        zoom_level_ = zoom_level;
        zoom_slider_->setValue(static_cast<int>(zoom_level * 100));
        zoom_label_->setText(QString("Zoom: %1%").arg(static_cast<int>(zoom_level * 100)));
        updateScrollArea();
        update();
    }
}

void GanttChartWidget::setTimeScale(int scale) {
    if (scale > 0) {
        time_scale_ = scale;
        updateScrollArea();
        update();
    }
}

void GanttChartWidget::zoomIn() {
    setZoomLevel(std::min(5.0, zoom_level_ * 1.2));
}

void GanttChartWidget::zoomOut() {
    setZoomLevel(std::max(0.1, zoom_level_ / 1.2));
}

void GanttChartWidget::resetZoom() {
    setZoomLevel(1.0);
}

void GanttChartWidget::scrollToTime(int time) {
    if (scroll_area_) {
        int x_pos = static_cast<int>(time * time_scale_ * zoom_level_);
        scroll_area_->horizontalScrollBar()->setValue(x_pos);
    }
}

void GanttChartWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    // Painting is handled by the drawing area
}

void GanttChartWidget::mousePressEvent(QMouseEvent* event) {
    Q_UNUSED(event)
    // Mouse handling is delegated to the drawing area
}

void GanttChartWidget::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        // Zoom with Ctrl+Wheel
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}

QSize GanttChartWidget::sizeHint() const {
    return QSize(800, 300);
}

void GanttChartWidget::setupUI() {
    main_layout_ = new QVBoxLayout(this);
    main_layout_->setContentsMargins(5, 5, 5, 5);
    
    setupControls();
    
    // Create scroll area
    scroll_area_ = new QScrollArea(this);
    scroll_area_->setWidgetResizable(true);
    scroll_area_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll_area_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Create drawing area
    drawing_area_ = new GanttDrawingArea(this);
    scroll_area_->setWidget(drawing_area_);
    
    main_layout_->addWidget(scroll_area_);
    
    updateScrollArea();
}

void GanttChartWidget::setupControls() {
    control_layout_ = new QHBoxLayout();
    
    // Zoom controls
    zoom_in_button_ = new QPushButton("Zoom In", this);
    zoom_out_button_ = new QPushButton("Zoom Out", this);
    reset_zoom_button_ = new QPushButton("Reset", this);
    
    connect(zoom_in_button_, &QPushButton::clicked, this, &GanttChartWidget::zoomIn);
    connect(zoom_out_button_, &QPushButton::clicked, this, &GanttChartWidget::zoomOut);
    connect(reset_zoom_button_, &QPushButton::clicked, this, &GanttChartWidget::resetZoom);
    
    // Zoom slider
    zoom_slider_ = new QSlider(Qt::Horizontal, this);
    zoom_slider_->setRange(10, 500); // 0.1x to 5.0x zoom
    zoom_slider_->setValue(100); // 1.0x zoom
    zoom_slider_->setFixedWidth(150);
    
    connect(zoom_slider_, &QSlider::valueChanged, this, [this](int value) {
        setZoomLevel(value / 100.0);
    });
    
    // Labels
    zoom_label_ = new QLabel("Zoom: 100%", this);
    time_label_ = new QLabel("Time: 0-100", this);
    
    // Layout controls
    control_layout_->addWidget(zoom_in_button_);
    control_layout_->addWidget(zoom_out_button_);
    control_layout_->addWidget(reset_zoom_button_);
    control_layout_->addWidget(zoom_slider_);
    control_layout_->addWidget(zoom_label_);
    control_layout_->addStretch();
    control_layout_->addWidget(time_label_);
    
    main_layout_->addLayout(control_layout_);
}

void GanttChartWidget::updateScrollArea() {
    if (drawing_area_) {
        int width = static_cast<int>((max_time_ + 20) * time_scale_ * zoom_level_);
        int height = static_cast<int>((process_order_.size() * row_height_ + axis_height_ + 2 * margin_));
        drawing_area_->setFixedSize(width, height);
        
        time_label_->setText(QString("Time: 0-%1").arg(max_time_));
    }
}

void GanttChartWidget::drawChart(QPainter& painter) {
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Clear background
    painter.fillRect(rect(), Qt::white);
    
    // Draw time axis
    drawTimeAxis(painter);
    
    // Draw process blocks
    drawProcessBlocks(painter);
    
    // Draw time markers
    drawTimeMarkers(painter);
}

void GanttChartWidget::drawTimeAxis(QPainter& painter) {
    painter.setPen(QPen(Qt::black, 2));
    
    int y = margin_;
    int axis_y = y + axis_height_;
    
    // Draw main axis line
    painter.drawLine(margin_, axis_y, 
                    static_cast<int>(margin_ + max_time_ * time_scale_ * zoom_level_), axis_y);
    
    // Draw time markers and labels
    painter.setFont(QFont("Arial", 8));
    int step = std::max(1, static_cast<int>(10 / zoom_level_));
    
    for (int time = 0; time <= max_time_; time += step) {
        int x = static_cast<int>(margin_ + time * time_scale_ * zoom_level_);
        
        // Draw tick mark
        painter.drawLine(x, axis_y - 5, x, axis_y + 5);
        
        // Draw time label
        painter.drawText(x - 10, axis_y - 8, 20, 15, Qt::AlignCenter, QString::number(time));
    }
}

void GanttChartWidget::drawProcessBlocks(QPainter& painter) {
    for (const auto& block : time_blocks_) {
        int row = getProcessRow(block.process_id);
        if (row == -1) continue;
        
        int x = static_cast<int>(margin_ + block.start_time * time_scale_ * zoom_level_);
        int y = margin_ + axis_height_ + row * row_height_ + 5;
        int width = static_cast<int>((block.end_time - block.start_time) * time_scale_ * zoom_level_);
        int height = row_height_ - 10;
        
        // Draw block
        QColor color = getProcessColor(block.process_id);
        painter.fillRect(x, y, width, height, color);
        
        // Draw border
        painter.setPen(QPen(color.darker(150), 1));
        painter.drawRect(x, y, width, height);
        
        // Draw process label if block is wide enough
        if (width > 30) {
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 9, QFont::Bold));
            QString label = getProcessLabel(block.process_id);
            painter.drawText(x + 2, y + 2, width - 4, height - 4, 
                           Qt::AlignCenter, label);
        }
    }
}

void GanttChartWidget::drawTimeMarkers(QPainter& painter) {
    // Draw vertical grid lines
    painter.setPen(QPen(Qt::lightGray, 1, Qt::DotLine));
    
    int step = std::max(1, static_cast<int>(5 / zoom_level_));
    int chart_height = margin_ + axis_height_ + process_order_.size() * row_height_;
    
    for (int time = 0; time <= max_time_; time += step) {
        int x = static_cast<int>(margin_ + time * time_scale_ * zoom_level_);
        painter.drawLine(x, margin_ + axis_height_, x, chart_height);
    }
}

QColor GanttChartWidget::getProcessColor(int process_id) const {
    int color_index = process_id % process_colors_.size();
    return process_colors_[color_index];
}

QString GanttChartWidget::getProcessLabel(int process_id) const {
    auto it = process_names_.find(process_id);
    if (it != process_names_.end() && !it->second.isEmpty()) {
        return it->second;
    }
    return QString("P%1").arg(process_id);
}

int GanttChartWidget::getProcessRow(int process_id) const {
    auto it = std::find(process_order_.begin(), process_order_.end(), process_id);
    if (it != process_order_.end()) {
        return static_cast<int>(std::distance(process_order_.begin(), it));
    }
    return -1;
}

void GanttChartWidget::ensureProcessExists(int process_id) {
    if (std::find(process_order_.begin(), process_order_.end(), process_id) == process_order_.end()) {
        process_order_.push_back(process_id);
        std::sort(process_order_.begin(), process_order_.end());
    }
}

GanttChartWidget::TimeBlock* GanttChartWidget::findTimeBlockAt(const QPoint& position) {
    for (auto& block : time_blocks_) {
        int row = getProcessRow(block.process_id);
        if (row == -1) continue;
        
        int x = static_cast<int>(margin_ + block.start_time * time_scale_ * zoom_level_);
        int y = margin_ + axis_height_ + row * row_height_ + 5;
        int width = static_cast<int>((block.end_time - block.start_time) * time_scale_ * zoom_level_);
        int height = row_height_ - 10;
        
        QRect block_rect(x, y, width, height);
        if (block_rect.contains(position)) {
            return &block;
        }
    }
    return nullptr;
}

// GanttDrawingArea implementation
GanttChartWidget::GanttDrawingArea::GanttDrawingArea(GanttChartWidget* parent)
    : QWidget(parent), gantt_widget_(parent) {
}

void GanttChartWidget::GanttDrawingArea::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    QPainter painter(this);
    gantt_widget_->drawChart(painter);
}

void GanttChartWidget::GanttDrawingArea::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        TimeBlock* block = gantt_widget_->findTimeBlockAt(event->pos());
        if (block) {
            emit gantt_widget_->timeBlockClicked(block->process_id, block->start_time, block->end_time);
        }
    }
    QWidget::mousePressEvent(event);
}

QSize GanttChartWidget::GanttDrawingArea::sizeHint() const {
    return QSize(800, 200);
}

#include "GanttChartWidget.moc"