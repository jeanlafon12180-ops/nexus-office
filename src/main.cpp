#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Nexus Office");
    QApplication::setOrganizationName("Nexus");

    MainWindow window;
    window.show();

    return app.exec();
}
