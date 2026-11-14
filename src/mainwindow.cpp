#include "mainwindow.h"
#include "writer/writerpage.h"
#include "sheets/sheetpage.h"
#include "slides/slidespage.h"
#include "pdf/pdfpage.h"

#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

namespace {
QLabel *makeLabel(const QString &text, const QString &objectName, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName(objectName);
    return label;
}

QPushButton *makeButton(const QString &text, const QString &objectName, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    button->setObjectName(objectName);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Nexus Office");
    setMinimumSize(900, 620);
    resize(1180, 760);

    auto *central = new QWidget(this);
    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *sidebar = new QWidget(central);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(238);

    auto *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(20, 28, 20, 22);
    sideLayout->setSpacing(10);

    auto *brand = makeLabel("NEXUS OFFICE", "brand", sidebar);
    sideLayout->addWidget(brand);
    sideLayout->addWidget(makeLabel("Votre espace de travail. Une suite.", "sidebarMuted", sidebar));
    sideLayout->addSpacing(28);
    sideLayout->addWidget(makeLabel("ESPACE DE TRAVAIL", "sidebarSection", sidebar));

    pages = new QStackedWidget(central);

    const QStringList names = {"Accueil", "Writer", "Sheets", "Slides", "PDF"};
    const QStringList descriptions = {
        "Vue d’ensemble de votre espace de travail",
        "Créer et modifier des documents texte",
        "Organiser les données et effectuer des calculs",
        "Créer des présentations",
        "Consulter et gérer les fichiers PDF"
    };

    for (int i = 0; i < names.size(); ++i) {
        auto *button = makeButton(names[i], "navButton", sidebar);
        button->setCheckable(true);
        sideLayout->addWidget(button);
        connect(button, &QPushButton::clicked, this, [this, i]() {
            pages->setCurrentIndex(i);
            for (auto *nav : findChildren<QPushButton *>("navButton"))
                nav->setChecked(nav->text() == QStringList{"Accueil", "Writer", "Sheets", "Slides", "PDF"}.at(i));
        });

        if (i == 0)
            button->setChecked(true);

    }

    // Build the dashboard and module landing pages.
    auto buildHome = [this, descriptions]() {
        auto *page = new QWidget;
        auto *layout = new QVBoxLayout(page);
        layout->setContentsMargins(42, 38, 42, 38);
        layout->setSpacing(16);

        layout->addWidget(makeLabel("Bonjour !", "pageHeading", page));
        layout->addWidget(makeLabel("Tout ce qu’il vous faut pour travailler.", "muted", page));
        layout->addSpacing(20);
        layout->addWidget(makeLabel("Votre espace de travail", "sectionHeading", page));

        const QStringList modules = {"Writer", "Sheets", "Slides", "PDF"};
        const QStringList summaries = {
            descriptions.at(1), descriptions.at(2),
            descriptions.at(3), descriptions.at(4)
        };

        for (int i = 0; i < modules.size(); ++i) {
            auto *card = new QFrame(page);
            card->setObjectName("moduleCard");
            auto *cardLayout = new QHBoxLayout(card);
            cardLayout->setContentsMargins(18, 14, 18, 14);

            auto *textColumn = new QVBoxLayout;
            textColumn->addWidget(makeLabel(modules.at(i), "cardTitle", card));
            textColumn->addWidget(makeLabel(summaries.at(i), "muted", card));
            cardLayout->addLayout(textColumn, 1);

            auto *open = makeButton("Ouvrir", "secondaryButton", card);
            cardLayout->addWidget(open);
            connect(open, &QPushButton::clicked, this, [this, i]() {
                pages->setCurrentIndex(i + 1);
                const auto navButtons = findChildren<QPushButton *>("navButton");
                if (i + 1 < navButtons.size())
                    navButtons.at(i + 1)->setChecked(true);
            });
            layout->addWidget(card);
        }

        layout->addStretch();
        auto *notice = makeLabel(
            "Aperçu : Writer permet de créer et modifier des documents. "
            "La compatibilité DOCX nécessite LibreOffice installé sur votre ordinateur.",
            "notice", page);
        notice->setWordWrap(true);
        layout->addWidget(notice);
        return page;
    };

    pages->addWidget(buildHome());
    pages->addWidget(new WriterPage);
    pages->addWidget(new SheetPage);
    pages->addWidget(new SlidesPage);
    pages->addWidget(new PdfPage);

    root->addWidget(sidebar);
    root->addWidget(pages, 1);
    setCentralWidget(central);
    applyTheme(true);
}

void MainWindow::applyTheme(bool dark)
{
    if (dark) {
        setStyleSheet(R"(
            QMainWindow, QWidget { background: #111827; color: #f9fafb; }
            QWidget#sidebar { background: #0b1220; }
            QLabel#brand { color: #ffffff; font-size: 20px; font-weight: 700; }
            QLabel#sidebarMuted, QLabel#muted, QLabel#notice { color: #9ca3af; }
            QLabel#sidebarSection { color: #6b7280; font-size: 10px; font-weight: 700; }
            QLabel#pageHeading { font-size: 28px; font-weight: 700; }
            QLabel#sectionHeading { font-size: 18px; font-weight: 600; }
            QLabel#cardTitle { font-size: 16px; font-weight: 700; }
            QPushButton#navButton { text-align: left; padding: 11px 12px; border: 0; border-radius: 8px; color: #cbd5e1; }
            QPushButton#navButton:hover, QPushButton#navButton:checked { background: #1f2937; color: #ffffff; }
            QPushButton#secondaryButton { padding: 9px 16px; border: 1px solid #374151; border-radius: 8px; background: #1f2937; }
            QFrame#moduleCard { background: #1f2937; border: 1px solid #374151; border-radius: 12px; }
            QTextEdit, QPlainTextEdit, QLineEdit, QTableWidget { background: #1f2937; color: #f9fafb; border: 1px solid #374151; }
        )");
    } else {
        setStyleSheet(QString());
    }
}
