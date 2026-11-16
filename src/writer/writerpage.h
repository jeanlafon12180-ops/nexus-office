#pragma once

#include <QWidget>
#include <QString>

class QTextEdit;
class QLabel;
class QComboBox;

class WriterPage final : public QWidget
{
    Q_OBJECT

public:
    explicit WriterPage(QWidget *parent = nullptr);

signals:
    void documentOpened(const QString &path, const QString &module);

private:
    QTextEdit *editor_ = nullptr;
    QLabel *documentLabel_ = nullptr;
    QLabel *wordCountLabel_ = nullptr;
    QString currentFile_;

    bool maybeSave();
    bool saveToFile(const QString &path);
    void updateDocumentInfo();
    void newDocument();
    void openDocument();
    void saveDocument();
    void saveDocumentAs();
    void findText();
    void exportPdf();
    void insertTable();
    void insertImage();
    void applyParagraphStyle(int index);
    void insertList(bool numbered);
    void printDocument();
    void checkSpelling();
};
