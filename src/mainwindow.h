#pragma once

#include <QMainWindow>

class QStackedWidget;

class MainWindow final : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *pages = nullptr;

    void applyTheme(bool dark);
};
