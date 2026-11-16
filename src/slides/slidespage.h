#pragma once
#include <QWidget>
#include <QVector>
#include <QString>
class QLineEdit;
class QTextEdit;
class QLabel;
class QTabWidget;
class QComboBox;
class QDialog;
class QStackedWidget;
class QPropertyAnimation;
class SlidesPage final : public QWidget {
    Q_OBJECT
public:
    explicit SlidesPage(QWidget *parent = nullptr);

signals:
    void documentOpened(const QString &path, const QString &module);
private:
    struct Slide { QWidget *page=nullptr; QLineEdit *title=nullptr; QTextEdit *body=nullptr; QTextEdit *notes=nullptr; };
    QVector<Slide> slides_;
    QTabWidget *tabs_=nullptr;
    QComboBox *theme_=nullptr;
    QComboBox *transition_=nullptr;
    QLabel *status_=nullptr;
    int nextSlideNumber_=2;
    Slide *currentSlide();
    void addSlide();
    void removeSlide();
    void insertImage();
    void insertTable();
    void insertShape();
    void applyTheme();
    void exportPdf();
    void startSlideshow();
    void importPptx();
    void exportPptx();
};
