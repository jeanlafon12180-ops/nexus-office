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
};
