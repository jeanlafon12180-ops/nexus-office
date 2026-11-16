#include "../src/pdf/pdfpage.h"

#include <QApplication>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QDebug>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    PdfPage page;
    if (!page.findChild<QSpinBox *>("pdfPageSpin")) return 1;
    const auto *zoom = page.findChild<QComboBox *>("pdfZoomCombo");
    if (!zoom || zoom->count() != 8) return 2;
    if (!page.findChild<QLineEdit *>("pdfSearchEdit")) return 3;
    if (!page.findChild<QWidget *>("pdfDocumentView")) return 4;

    const auto buttons = page.findChildren<QPushButton *>();
    QStringList labels;
    for (const auto *button : buttons) labels << button->text();
    for (const QString &required : {"Ouvrir", "Suivant", "Commentaire", "Surligner texte", "Exporter PDF annoté", "Imprimer"})
        if (!labels.contains(required)) return 5;

    qInfo() << "PDF UI structure test passed.";
    return 0;
}
