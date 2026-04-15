#pragma once

#include <QWidget>
#include <QPainter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <vector>
#include <map>

/**
 * @brief Gantt chart widget for visualizing process execution timeline
 * 
 * The GanttChartWidget displays a timeline showing when each process
 * executes, with different colors for each process and time markers.
 */
class GanttChartWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Represents a time block in the Gantt chart
     */
    struct TimeBlock {
        int process_id;
        int start_time;
        int end_time;
        QString process_name;
        
        TimeBlock(int pid, int start, int end, const QString& name = "")
            : process_id(pid), start_time(start), end_time(end), process_name(name) {}
    };

    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit GanttChartWidget(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~GanttChartWidget() = default;
    
    /**
     * @brief Add a time block for process execution
     * @param process_id Process ID
     * @param start_time Start time of execution
     * @param end_time End time of execution
     * @param process_name Optional process name
     */
    void addTimeBlock(int process_id, int start_time, int end_time, const QString& process_name = "");
    
    /**
     * @brief Clear all time blocks
     */
    void clearChart();
    
    /**
     * @brief Set the maximum time to display
     * @param max_time Maximum time value
     */
    void setMaxTime(int max_time);
    
    /**
     * @brief Get the maximum time displayed
     * @return Maximum time value
     */
    int getMaxTime() const { return max_time_; }
    
    /**
     * @brief Set zoom level
     * @param zoom_level Zoom level (1.0 = normal, 2.0 = 2x zoom, etc.)
     */
    void setZoomLevel(double zoom_level);
    
    /**
     * @brief Get current zoom level
     * @return Current zoom level
     */
    double getZoomLevel() const { return zoom_level_; }
    
    /**
     * @brief Set time unit scale (pixels per time unit)
     * @param scale Pixels per time unit
     */
    void setTimeScale(int scale);
    
    /**
     * @brief Get time unit scale
     * @return Pixels per time unit
     */
    int getTimeScale() const { return time_scale_; }

public slots:
    /**
     * @brief Zoom in
     */
    void zoomIn();
    
    /**
     * @brief Zoom out
     */
    void zoomOut();
    
    /**
     * @brief Reset zoom to normal
     */
    void resetZoom();
    
    /**
     * @brief Scroll to specific time
     * @param time Time to scroll to
     */
    void scrollToTime(int time);

signals:
    /**
     * @brief Emitted when a time block is clicked
     * @param process_id Process ID of clicked block
     * @param start_time Start time of block
     * @param end_time End time of block
     */
    void timeBlockClicked(int process_id, int start_time, int end_time);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    QSize sizeHint() const override;

private:
    /**
     * @brief Custom widget for drawing the Gantt chart
     */
    class GanttDrawingArea : public QWidget {
    public:
        explicit GanttDrawingArea(GanttChartWidget* parent);
        
    protected:
        void paintEvent(QPaintEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        QSize sizeHint() const override;
        
    private:
        GanttChartWidget* gantt_widget_;
    };
    
    void setupUI();
    void setupControls();
    void updateScrollArea();
    void drawChart(QPainter& painter);
    void drawTimeAxis(QPainter& painter);
    void drawProcessBlocks(QPainter& painter);
    void drawTimeMarkers(QPainter& painter);
    QColor getProcessColor(int process_id) const;
    QString getProcessLabel(int process_id) const;
    int getProcessRow(int process_id) const;
    void ensureProcessExists(int process_id);
    TimeBlock* findTimeBlockAt(const QPoint& position);
    
    // Data
    std::vector<TimeBlock> time_blocks_;
    std::map<int, QString> process_names_;
    std::vector<int> process_order_;
    
    // Display settings
    int max_time_;
    double zoom_level_;
    int time_scale_;
    int row_height_;
    int axis_height_;
    int margin_;
    
    // UI components
    QVBoxLayout* main_layout_;
    QHBoxLayout* control_layout_;
    QScrollArea* scroll_area_;
    GanttDrawingArea* drawing_area_;
    
    // Controls
    QPushButton* zoom_in_button_;
    QPushButton* zoom_out_button_;
    QPushButton* reset_zoom_button_;
    QSlider* zoom_slider_;
    QLabel* zoom_label_;
    QLabel* time_label_;
    
    // Colors for processes
    static const std::vector<QColor> process_colors_;
};