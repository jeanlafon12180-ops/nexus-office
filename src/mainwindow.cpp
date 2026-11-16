#include "mainwindow.h"
#include "writer/writerpage.h"
#include "sheets/sheetpage.h"
#include "slides/slidespage.h"
#include "pdf/pdfpage.h"

#include <QAction>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QStackedWidget>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <QDesktopServices>
#include <QUrl>

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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Nexus Office");
    setMinimumSize(900, 620);
    resize(1180, 760);
    loadPreferences();

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
    sideLayout->addWidget(makeLabel("NEXUS OFFICE", "brand", sidebar));
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
        connect(button, &QPushButton::clicked, this, [this, i, names]() {
            pages->setCurrentIndex(i);
            for (auto *nav : findChildren<QPushButton *>("navButton"))
                nav->setChecked(nav->text() == names.at(i));
        });
        if (i == 0) button->setChecked(true);
    }
    sideLayout->addStretch();
    auto *settingsButton = makeButton("Paramètres", "settingsButton", sidebar);
    sideLayout->addWidget(settingsButton);
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::showSettings);
    auto *settingsShortcut = new QAction(this);
    settingsShortcut->setShortcut(QKeySequence("Ctrl+,"));
    addAction(settingsShortcut);
    connect(settingsShortcut, &QAction::triggered, this, &MainWindow::showSettings);

    auto *writer = new WriterPage;
    auto *sheets = new SheetPage;
    auto *slides = new SlidesPage;
    auto *pdf = new PdfPage;
    connect(writer, &WriterPage::documentOpened, this, &MainWindow::addRecentDocument);
    connect(sheets, &SheetPage::documentOpened, this, &MainWindow::addRecentDocument);
    connect(slides, &SlidesPage::documentOpened, this, &MainWindow::addRecentDocument);
    connect(pdf, &PdfPage::documentOpened, this, &MainWindow::addRecentDocument);

    auto buildHome = [this, descriptions]() {
        auto *page = new QWidget;
        auto *layout = new QVBoxLayout(page);
        layout->setContentsMargins(42, 38, 42, 38);
        layout->setSpacing(16);
        layout->addWidget(makeLabel("Bonjour !", "pageHeading", page));
        layout->addWidget(makeLabel("Tout ce qu’il vous faut pour travailler.", "muted", page));
        layout->addSpacing(16);
        layout->addWidget(makeLabel("Votre espace de travail", "sectionHeading", page));

        const QStringList modules = {"Writer", "Sheets", "Slides", "PDF"};
        for (int i = 0; i < modules.size(); ++i) {
            auto *card = new QFrame(page);
            card->setObjectName("moduleCard");
            auto *cardLayout = new QHBoxLayout(card);
            cardLayout->setContentsMargins(18, 12, 18, 12);
            auto *textColumn = new QVBoxLayout;
            textColumn->addWidget(makeLabel(modules.at(i), "cardTitle", card));
            textColumn->addWidget(makeLabel(descriptions.at(i + 1), "muted", card));
            cardLayout->addLayout(textColumn, 1);
            auto *open = makeButton("Ouvrir", "secondaryButton", card);
            cardLayout->addWidget(open);
            connect(open, &QPushButton::clicked, this, [this, i]() {
                pages->setCurrentIndex(i + 1);
                const auto navButtons = findChildren<QPushButton *>("navButton");
                if (i + 1 < navButtons.size()) navButtons.at(i + 1)->setChecked(true);
            });
            layout->addWidget(card);
        }
        layout->addSpacing(12);
        layout->addWidget(makeLabel("Documents récents", "sectionHeading", page));
        recentList_ = new QListWidget(page);
        recentList_->setObjectName("recentDocumentsList");
        recentList_->setMaximumHeight(150);
        layout->addWidget(recentList_);
        connect(recentList_, &QListWidget::itemDoubleClicked, this, &MainWindow::openRecentDocument);
        auto *notice = makeLabel("Les fichiers restent sur votre appareil. Les conversions DOCX, XLSX et PPTX peuvent nécessiter LibreOffice.", "notice", page);
        notice->setWordWrap(true);
        layout->addWidget(notice);
        layout->addStretch();
        return page;
    };

    pages->addWidget(buildHome());
    pages->addWidget(writer);
    pages->addWidget(sheets);
    pages->addWidget(slides);
    pages->addWidget(pdf);
    root->addWidget(sidebar);
    root->addWidget(pages, 1);
    setCentralWidget(central);
    applyTheme(darkTheme_);

    QSettings settings;
    const QStringList recent = settings.value("recentDocuments").toStringList();
    for (const QString &entry : recent) {
        const QStringList parts = entry.split('|');
        if (parts.size() == 2 && QFileInfo::exists(parts.at(0)))
            recentList_->addItem(parts.at(1) + " — " + QFileInfo(parts.at(0)).fileName() + "\n" + parts.at(0));
    }
}

void MainWindow::loadPreferences()
{
    QSettings settings;
    darkTheme_ = settings.value("appearance/darkTheme", true).toBool();
}

void MainWindow::savePreferences()
{
    QSettings settings;
    settings.setValue("appearance/darkTheme", darkTheme_);
    settings.sync();
}

void MainWindow::addRecentDocument(const QString &path, const QString &module)
{
    if (path.isEmpty()) return;
    QSettings settings;
    QStringList entries = settings.value("recentDocuments").toStringList();
    const QString entry = path + "|" + module;
    entries.removeAll(entry);
    entries.prepend(entry);
    while (entries.size() > 12) entries.removeLast();
    settings.setValue("recentDocuments", entries);
    if (!recentList_) return;
    recentList_->clear();
    for (const QString &saved : entries) {
        const QStringList parts = saved.split('|');
        if (parts.size() == 2 && QFileInfo::exists(parts.at(0)))
            recentList_->addItem(parts.at(1) + " — " + QFileInfo(parts.at(0)).fileName() + "\n" + parts.at(0));
    }
}

void MainWindow::openRecentDocument(QListWidgetItem *item)
{
    if (!item) return;
    const QString path = item->text().section('\n', 1);
    if (!QFileInfo::exists(path)) {
        QMessageBox::warning(this, "Document récent", "Ce fichier n’existe plus à cet emplacement.");
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::showSettings()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Paramètres de Nexus Office");
    auto *layout = new QVBoxLayout(&dialog);
    auto *appearance = new QComboBox(&dialog);
    appearance->addItem("Sombre", true);
    appearance->addItem("Clair", false);
    appearance->setCurrentIndex(darkTheme_ ? 0 : 1);
    layout->addWidget(new QLabel("Apparence", &dialog));
    layout->addWidget(appearance);
    auto *paper = new QComboBox(&dialog);
    paper->addItems({"A4", "Letter", "A3"});
    auto *orientation = new QComboBox(&dialog);
    orientation->addItems({"Portrait", "Paysage"});
    QSettings current;
    paper->setCurrentText(current.value("document/paperSize", "A4").toString());
    orientation->setCurrentText(current.value("document/orientation", "Portrait").toString());
    layout->addWidget(new QLabel("Format de papier par défaut", &dialog));
    layout->addWidget(paper);
    layout->addWidget(new QLabel("Orientation par défaut", &dialog));
    layout->addWidget(orientation);
    layout->addWidget(new QLabel("Les préférences sont enregistrées sur cet appareil.", &dialog));
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    darkTheme_ = appearance->currentData().toBool();
    applyTheme(darkTheme_);
    savePreferences();
    QSettings settings;
    settings.setValue("document/paperSize", paper->currentText());
    settings.setValue("document/orientation", orientation->currentText());
    settings.sync();
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
            QPushButton#secondaryButton, QPushButton#settingsButton { padding: 9px 16px; border: 1px solid #374151; border-radius: 8px; background: #1f2937; }
            QFrame#moduleCard { background: #1f2937; border: 1px solid #374151; border-radius: 12px; }
            QTextEdit, QPlainTextEdit, QLineEdit, QTableWidget, QListWidget, QComboBox { background: #1f2937; color: #f9fafb; border: 1px solid #374151; }
        )");
    } else {
        setStyleSheet(QString());
    }
}
