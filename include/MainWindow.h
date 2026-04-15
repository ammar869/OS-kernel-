#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

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
    
    // UI components will be added here as we implement them
    QWidget* central_widget_;
    QVBoxLayout* main_layout_;
    QLabel* placeholder_label_;
};