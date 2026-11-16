#pragma once

#include <QWidget>
#include <QString>

class QLabel;
class QLineEdit;
class QPdfDocument;
class QPdfView;
class QSpinBox;
class QComboBox;
class QPushButton;

class PdfPage final : public QWidget
{
    Q_OBJECT
public:
    explicit PdfPage(QWidget *parent = nullptr);

private:
    QPdfDocument *document_ = nullptr;
    QPdfView *view_ = nullptr;
    QLabel *fileLabel_ = nullptr;
    QLabel *statusLabel_ = nullptr;
    QSpinBox *pageSpin_ = nullptr;
    QComboBox *zoomCombo_ = nullptr;
    QLineEdit *searchEdit_ = nullptr;
    QPushButton *notesButton_ = nullptr;
    QPushButton *highlightButton_ = nullptr;
    QPushButton *exportButton_ = nullptr;
    QString currentPath_;
    int lastSearchPage_ = -1;

    void openPdf();
    void updateDocumentState();
    void findNext();
    void editPageNote();
    void highlightText();
    void exportAnnotatedPdf();
    void printPdf();
    QString notesPath() const;
    QString highlightsPath() const;
};
