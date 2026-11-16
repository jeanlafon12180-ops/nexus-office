#pragma once

#include <QMainWindow>
#include <QString>

class QStackedWidget;
class QListWidget;
class QLabel;
class QSettings;

class MainWindow final : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *pages = nullptr;
    QListWidget *recentList_ = nullptr;
    QLabel *themeStatus_ = nullptr;
    bool darkTheme_ = true;

    void applyTheme(bool dark);
    void loadPreferences();
    void savePreferences();
    void addRecentDocument(const QString &path, const QString &module);
    void openRecentDocument(QListWidget *item);
    void showSettings();
};
