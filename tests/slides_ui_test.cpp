#include "../src/slides/slidespage.h"

#include <QApplication>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QDebug>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    SlidesPage page;
    const auto *tabs=page.findChild<QTabWidget *>("slidesTabs");
    const auto *transition=page.findChild<QComboBox *>("slideTransition");
    const auto *theme=page.findChild<QComboBox *>("slideTheme");
    if(!tabs || tabs->count()!=1) return 1;
    if(!transition || transition->count()!=3
       || transition->itemText(1)!="Fondu"
       || transition->itemText(2)!="Glissement") return 2;
    if(!theme || theme->count()!=3) return 3;
    const auto buttons=page.findChildren<QPushButton *>();
    QStringList labels;
    for(const auto *button:buttons) labels<<button->text();
    for(const QString &required:{"Diaporama","Ouvrir PPTX","Enregistrer PPTX","Exporter PDF",
                                  "Insérer une image","Insérer un tableau","Insérer une forme"}) {
        if(!labels.contains(required)) return 4;
    }
    qInfo()<<"Slides UI structure test passed.";
    return 0;
}
