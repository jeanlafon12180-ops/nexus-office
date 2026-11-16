#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Nexus Office");
    QApplication::setOrganizationName("Nexus");
    app.setWindowIcon(QIcon(":/icons/nexus-office.svg"));

    MainWindow window;
    window.show();

    return app.exec();
}
