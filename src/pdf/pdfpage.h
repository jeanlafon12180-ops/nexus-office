#pragma once
#include <QWidget>
class QLabel;
class PdfPage final : public QWidget {
    Q_OBJECT
public:
    explicit PdfPage(QWidget *parent = nullptr);
private:
    QLabel *fileLabel_ = nullptr;
    void openPdf();
};
