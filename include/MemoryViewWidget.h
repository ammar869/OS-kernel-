#pragma once

#include "MemoryTypes.h"
#include <QWidget>
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QScrollArea>
#include <vector>
#include <map>

/**
 * @brief Widget for displaying a single memory frame
 */
class FrameWidget : public QFrame {
    Q_OBJECT

public:
    explicit FrameWidget(int frame_id, QWidget* parent = nullptr);

    void setFrameData(const PageFrame& frame);
    void setHighlighted(bool highlighted);
    bool isHighlighted() const { return highlighted_; }
    int getFrameId() const { return frame_id_; }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

signals:
    void clicked(int frame_id);

private:
    int frame_id_;
    PageFrame frame_data_;
    bool highlighted_;
    QLabel* label_;
};

/**
 * @brief Widget for visualizing memory frame allocation and page tables
 */
class MemoryViewWidget : public QWidget {
    Q_OBJECT

public:
    explicit MemoryViewWidget(QWidget* parent = nullptr);
    ~MemoryViewWidget() = default;

    void updateMemoryFrames(const std::vector<PageFrame>& frames);
    void updatePageTable(int process_id, const std::vector<PageTableEntry>& page_table);
    void setSelectedProcess(int process_id);
    int getSelectedProcess() const { return selected_process_; }
    void highlightFrame(int frame_id);
    void clearMemory();
    void setMemoryConfiguration(const MemoryConfiguration& config);

    static const std::vector<QColor> process_colors_;

public slots:
    void onProcessSelectionChanged(int index);

signals:
    void frameClicked(int frame_id);
    void selectedProcessChanged(int process_id);

private:
    void setupUI();
    void setupMemoryGrid();
    void setupPageTableView();
    void updateProcessSelector();
    QColor getProcessColor(int process_id) const;

    QVBoxLayout* main_layout_;
    QHBoxLayout* top_layout_;

    QGroupBox* memory_group_;
    QGridLayout* memory_grid_;
    std::vector<FrameWidget*> frame_widgets_;

    QGroupBox* page_table_group_;
    QVBoxLayout* page_table_layout_;
    QComboBox* process_selector_;
    QTableWidget* page_table_;

    QLabel* memory_info_label_;
    QLabel* utilization_label_;

    std::vector<PageFrame> current_frames_;
    std::map<int, std::vector<PageTableEntry>> process_page_tables_;
    MemoryConfiguration memory_config_;
    int selected_process_;
    int highlighted_frame_;
    int frames_per_row_;
};