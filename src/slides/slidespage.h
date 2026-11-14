#pragma once
#include <QWidget>
class QLineEdit;
class QTextEdit;
class QLabel;
class SlidesPage final : public QWidget {
    Q_OBJECT
public:
    explicit SlidesPage(QWidget *parent = nullptr);
private:
    QLineEdit *title_ = nullptr;
    QTextEdit *body_ = nullptr;
    QLabel *status_ = nullptr;
    void exportPdf();
};
