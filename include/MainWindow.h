#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>

/**
 * @brief Main application window
 * 
 * The main window contains all GUI components and manages the overall
 * application layout and user interface.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~MainWindow();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    
    // UI components
    QWidget* central_widget_;
    QVBoxLayout* main_layout_;
    QLabel* placeholder_label_;
    
    // Panel containers
    QGroupBox* dashboard_panel_;
    QGroupBox* process_table_panel_;
    QGroupBox* gantt_chart_panel_;
    QGroupBox* memory_view_panel_;
    QGroupBox* metrics_panel_panel_;
    
    // Actual widget components (placeholders for now)
    QWidget* dashboard_;
    QWidget* process_table_;
    QWidget* gantt_chart_;
    QWidget* memory_view_;
    QWidget* metrics_panel_;
};