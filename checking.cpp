#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QLabel label("Hello Qt GUI");
    label.resize(300, 200);
    label.show();

    return app.exec();
}