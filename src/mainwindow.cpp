#include "mainwindow.h"
#include "writer/writerpage.h"

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

    auto buildModule = [this](const QString &name, const QString &description) {
        auto *page = new QWidget;
        auto *layout = new QVBoxLayout(page);
        layout->setContentsMargins(42, 38, 42, 38);
        layout->setSpacing(16);

        layout->addWidget(makeLabel(name, "pageHeading", page));
        layout->addWidget(makeLabel(description, "muted", page));
        layout->addSpacing(18);

        auto *status = new QFrame(page);
        status->setObjectName("modulePanel");
        auto *statusLayout = new QVBoxLayout(status);
        statusLayout->setContentsMargins(22, 22, 22, 22);
        statusLayout->addWidget(makeLabel("Cet espace est en cours de développement", "cardTitle", status));
        auto *detail = makeLabel(
            "Cette interface est en préparation. Les fonctions de tableur, de présentation "
            "et de gestion PDF seront ajoutées progressivement.",
            "muted", status);
        detail->setWordWrap(true);
        statusLayout->addWidget(detail);
        layout->addWidget(status);

        auto *chooseFile = makeButton("Choisir un fichier…", "primaryButton", page);
        layout->addWidget(chooseFile, 0, Qt::AlignLeft);
        connect(chooseFile, &QPushButton::clicked, this, [this, name]() {
            const QString file = QFileDialog::getOpenFileName(
                this, "Choisir un fichier pour " + name, QString(),
                "Office documents (*.odt *.ods *.odp *.docx *.xlsx *.pptx *.pdf);;All files (*)");
            if (!file.isEmpty()) {
                QMessageBox::information(
                    this, "Fichier sélectionné",
                    "Selected file:
" + file +
                    "

The document engine is not integrated yet. "
                    "This file has not been opened or modified.");
            }
        });

        layout->addStretch();
        return page;
    };

    pages->addWidget(buildHome());
    pages->addWidget(new WriterPage);
    pages->addWidget(buildModule("Nexus Sheets", "Un espace pour les feuilles de calcul et les formules."));
    pages->addWidget(buildModule("Nexus Slides", "Un espace pour créer des présentations."));
    pages->addWidget(buildModule("Nexus PDF", "Un espace pour consulter et gérer vos PDF."));


    sideLayout->addStretch();

    auto *themeButton = makeButton("Passer en mode sombre", "themeButton", sidebar);
    sideLayout->addWidget(themeButton);
    sideLayout->addWidget(makeLabel("Nexus Office · Aperçu 0.2.0", "sidebarMuted", sidebar));
    connect(themeButton, &QPushButton::clicked, this, [this, themeButton]() {
        const bool dark = property("darkMode").toBool();
        applyTheme(!dark);
        themeButton->setText(dark ? "Passer en mode sombre" : "Passer en mode clair");
    });

    root->addWidget(sidebar);
    root->addWidget(pages, 1);
    setCentralWidget(central);
    applyTheme(false);
}

void MainWindow::applyTheme(bool dark)
{
    setProperty("darkMode", dark);

    if (dark) {
        setStyleSheet(R"(
            QMainWindow, QWidget { background: #111827; color: #e5e7eb; }
            QLabel { background: transparent; border: 0; }
            QWidget#sidebar { background: #0b1220; }
            QLabel#brand { color: #ffffff; font-size: 19px; font-weight: 800; letter-spacing: 1px; }
            QLabel#sidebarMuted { color: #8793a8; font-size: 11px; }
            QLabel#sidebarSection { color: #64748b; font-size: 10px; font-weight: 700; }
            QLabel#pageHeading { color: #f9fafb; font-size: 32px; font-weight: 750; }
            QLabel#sectionHeading { color: #f3f4f6; font-size: 19px; font-weight: 650; }
            QLabel#muted { color: #9ca3af; font-size: 13px; }
            QLabel#cardTitle { color: #f9fafb; font-size: 16px; font-weight: 650; }
            QPushButton#navButton, QPushButton#themeButton {
                text-align: left; color: #cbd5e1; background: transparent;
                border: 0; border-radius: 8px; padding: 11px 12px;
            }
            QPushButton#navButton:hover, QPushButton#themeButton:hover,
            QPushButton#navButton:checked { background: #263449; color: white; }
            QFrame#moduleCard, QFrame#modulePanel {
                background: #1f2937; border: 1px solid #374151; border-radius: 12px;
            }
            QPushButton#secondaryButton {
                color: #e5e7eb; background: #374151; border: 0;
                border-radius: 8px; padding: 9px 16px;
            }
            QPushButton#primaryButton {
                color: white; background: #375ee8; border: 0;
                border-radius: 8px; padding: 11px 18px;
            }
            QLabel#notice {
                background: #1e3a5f; color: #dbeafe;
                border-radius: 10px; padding: 16px;
            }
        )");
    } else {
        setStyleSheet(R"(
            QMainWindow, QWidget { background: #f5f7fb; color: #172033; }
            QLabel { background: transparent; border: 0; }
            QWidget#sidebar { background: #111827; color: #f9fafb; }
            QLabel#brand { color: #ffffff; font-size: 19px; font-weight: 800; letter-spacing: 1px; }
            QLabel#sidebarMuted { color: #8b95a7; font-size: 11px; }
            QLabel#sidebarSection { color: #64748b; font-size: 10px; font-weight: 700; }
            QLabel#pageHeading { color: #111827; font-size: 32px; font-weight: 750; }
            QLabel#sectionHeading { color: #172033; font-size: 19px; font-weight: 650; }
            QLabel#muted { color: #7b8495; font-size: 13px; }
            QLabel#cardTitle { color: #172033; font-size: 16px; font-weight: 650; }
            QPushButton#navButton, QPushButton#themeButton {
                text-align: left; color: #d1d5db; background: transparent;
                border: 0; border-radius: 8px; padding: 11px 12px;
            }
            QPushButton#navButton:hover, QPushButton#themeButton:hover,
            QPushButton#navButton:checked { background: #263244; color: white; }
            QFrame#moduleCard, QFrame#modulePanel {
                background: white; border: 1px solid #e1e6ef; border-radius: 12px;
            }
            QPushButton#secondaryButton {
                color: #263449; background: #eef2f8; border: 0;
                border-radius: 8px; padding: 9px 16px;
            }
            QPushButton#primaryButton {
                color: white; background: #315be8; border: 0;
                border-radius: 8px; padding: 11px 18px;
            }
            QLabel#notice {
                background: #eaf1ff; color: #294b82;
                border-radius: 10px; padding: 16px;
            }
        )");
    }
}
