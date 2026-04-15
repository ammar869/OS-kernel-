#pragma once

#include "MemoryTypes.h"
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QComboBox>
#include <QTableWidget>
#include <QGroupBox>
#include <vector>
#include <map>

/**
 * @brief Widget for visualizing memory frame allocation and page tables
 * 
 * The MemoryViewWidget displays memory frames, their allocation status,
 * and page table mappings for processes.
 */
class MemoryViewWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit MemoryViewWidget(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~MemoryViewWidget() = default;
    
    /**
     * @brief Update memory frames display
     * @param frames Vector of page frames
     */
    void updateMemoryFrames(const std::vector<PageFrame>& frames);
    
    /**
     * @brief Update page table for a specific process
     * @param process_id Process ID
     * @param page_table Page table entries
     */
    void updatePageTable(int process_id, const std::vector<PageTableEntry>& page_table);
    
    /**
     * @brief Set selected process for page table display
     * @param process_id Process ID to display page table for
     */
    void setSelectedProcess(int process_id);
    
    /**
     * @brief Get currently selected process
     * @return Selected process ID
     */
    int getSelectedProcess() const { return selected_process_; }
    
    /**
     * @brief Highlight a specific frame (for page replacement visualization)
     * @param frame_id Frame ID to highlight, -1 to clear
     */
    void highlightFrame(int frame_id);
    
    /**
     * @brief Clear all memory data
     */
    void clearMemory();
    
    /**
     * @brief Set memory configuration
     * @param config Memory configuration
     */
    void setMemoryConfiguration(const MemoryConfiguration& config);

public slots:
    /**
     * @brief Handle process selection change
     * @param index Selected process index
     */
    void onProcessSelectionChanged(int index);

signals:
    /**
     * @brief Emitted when a memory frame is clicked
     * @param frame_id Frame ID that was clicked
     */
    void frameClicked(int frame_id);
    
    /**
     * @brief Emitted when selected process changes
     * @param process_id New selected process ID
     */
    void selectedProcessChanged(int process_id);

private:
    /**
     * @brief Custom widget for displaying a single memory frame
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
    
    void setupUI();
    void setupMemoryGrid();
    void setupPageTableView();
    void setupControls();
    void updateFrameWidget(int frame_id, const PageFrame& frame);
    void updateProcessSelector();
    QColor getProcessColor(int process_id) const;
    QString getFrameText(const PageFrame& frame) const;
    
    // UI components
    QVBoxLayout* main_layout_;
    QHBoxLayout* top_layout_;
    
    // Memory grid
    QGroupBox* memory_group_;
    QGridLayout* memory_grid_;
    std::vector<FrameWidget*> frame_widgets_;
    
    // Page table view
    QGroupBox* page_table_group_;
    QVBoxLayout* page_table_layout_;
    QComboBox* process_selector_;
    QTableWidget* page_table_;
    
    // Controls
    QLabel* memory_info_label_;
    QLabel* utilization_label_;
    
    // Data
    std::vector<PageFrame> current_frames_;
    std::map<int, std::vector<PageTableEntry>> process_page_tables_;
    MemoryConfiguration memory_config_;
    int selected_process_;
    int highlighted_frame_;
    
    // Layout settings
    int frames_per_row_;
    static const std::vector<QColor> process_colors_;
};