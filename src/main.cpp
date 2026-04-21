#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>
#include <QDebug>
#include <iostream>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    // Install Qt message handler to catch qFatal/qCritical
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& ctx, const QString& msg) {
        std::cerr << "[Qt] " << msg.toStdString() << std::endl;
        if (type == QtFatalMsg) abort();
    });

    QApplication app(argc, argv);
    app.setApplicationName("Mini OS Kernel Simulator");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Educational Software");
    app.setStyle(QStyleFactory::create("Fusion"));

    std::cerr << "[main] QApplication created\n";

    try {
        std::cerr << "[main] Creating MainWindow...\n";
        MainWindow* window = new MainWindow();
        std::cerr << "[main] MainWindow created, showing...\n";
        window->show();
        std::cerr << "[main] Entering event loop\n";
        int ret = app.exec();
        delete window;
        return ret;
    }
    catch (const std::exception& e) {
        std::cerr << "[main] std::exception: " << e.what() << std::endl;
        QMessageBox::critical(nullptr, "Fatal Error", e.what());
        return -1;
    }
    catch (...) {
        std::cerr << "[main] Unknown exception caught\n";
        QMessageBox::critical(nullptr, "Fatal Error", "Unknown fatal error");
        return -1;
    }
}
