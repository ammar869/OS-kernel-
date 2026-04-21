#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>
#include <iostream>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Mini OS Kernel Simulator");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Educational Software");
    app.setApplicationDisplayName("Mini OS Kernel Simulator");
    
    // Set application style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    try {
        // Create and show main window
        MainWindow window;
        window.show();
        
        return app.exec();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        QMessageBox::critical(nullptr, "Fatal Error", QString::fromStdString(e.what()));
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        QMessageBox::critical(nullptr, "Fatal Error", "Unknown fatal error occurred");
        return -1;
    }
}