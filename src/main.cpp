#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Mini OS Kernel Simulator");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Educational Software");
    app.setStyle(QStyleFactory::create("Fusion"));

    try {
        MainWindow* window = new MainWindow();
        window->show();
        int ret = app.exec();
        delete window;
        return ret;
    }
    catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Fatal Error", e.what());
        return -1;
    }
    catch (...) {
        QMessageBox::critical(nullptr, "Fatal Error", "Unknown fatal error occurred.");
        return -1;
    }
}
