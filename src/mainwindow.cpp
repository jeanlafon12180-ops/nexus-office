#include "mainwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Nexus Office");
    resize(1100, 720);
    setMinimumSize(800, 560);

    auto *central = new QWidget(this);
    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *sidebar = new QWidget(central);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(230);

    auto *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(22, 28, 22, 22);
    sideLayout->setSpacing(14);

    auto *brand = new QLabel("NEXUS OFFICE", sidebar);
    brand->setObjectName("brand");
    sideLayout->addWidget(brand);

    auto *tagline = new QLabel("Your workspace. One suite.", sidebar);
    tagline->setObjectName("muted");
    sideLayout->addWidget(tagline);
    sideLayout->addSpacing(24);

    for (const QString &name : {
             "Home", "Writer", "Sheets", "Slides", "PDF"
         }) {
        auto *button = new QPushButton(name, sidebar);
        button->setObjectName("navButton");
        button->setCursor(Qt::PointingHandCursor);
        sideLayout->addWidget(button);
    }

    sideLayout->addStretch();

    auto *version = new QLabel("Early preview · 0.1.0", sidebar);
    version->setObjectName("muted");
    sideLayout->addWidget(version);

    auto *content = new QWidget(central);
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(48, 42, 48, 42);
    contentLayout->setSpacing(18);

    auto *heading = new QLabel("Good day.", content);
    heading->setObjectName("heading");

    auto *description = new QLabel(
        "Create something great with Nexus Office.", content);
    description->setObjectName("muted");

    auto *section = new QLabel("Create a new document", content);
    section->setObjectName("sectionTitle");

    auto *actions = new QHBoxLayout;
    actions->setSpacing(14);

    for (const QString &name : {
             "New document", "New spreadsheet", "New presentation"
         }) {
        auto *button = new QPushButton(name, content);
        button->setMinimumHeight(90);
        button->setObjectName("actionCard");
        button->setEnabled(false); // Editors are not implemented yet.
        actions->addWidget(button);
    }

    auto *notice = new QLabel(
        "The desktop shell is under construction. Document editing "
        "and file opening are not available in this preview.", content);
    notice->setWordWrap(true);
    notice->setObjectName("notice");

    contentLayout->addWidget(heading);
    contentLayout->addWidget(description);
    contentLayout->addSpacing(28);
    contentLayout->addWidget(section);
    contentLayout->addLayout(actions);
    contentLayout->addSpacing(12);
    contentLayout->addWidget(notice);
    contentLayout->addStretch();

    root->addWidget(sidebar);
    root->addWidget(content, 1);
    setCentralWidget(central);

    setStyleSheet(R"(
        QMainWindow, QWidget { background: #f5f7fb; color: #172033; }
        QWidget#sidebar { background: #111827; color: #f9fafb; }
        QLabel#brand { color: #ffffff; font-size: 19px; font-weight: 800; letter-spacing: 1px; }
        QLabel#muted { color: #8b95a7; font-size: 12px; }
        QLabel#heading { font-size: 34px; font-weight: 700; color: #111827; }
        QLabel#sectionTitle { font-size: 18px; font-weight: 650; }
        QPushButton#navButton {
            text-align: left; color: #d1d5db; background: transparent;
            border: 0; border-radius: 8px; padding: 11px 12px;
        }
        QPushButton#navButton:hover { background: #263244; color: white; }
        QPushButton#actionCard {
            background: white; border: 1px solid #e1e6ef;
            border-radius: 12px; padding: 16px; font-weight: 600;
        }
        QPushButton#actionCard:disabled { color: #697386; }
        QLabel#notice {
            background: #eaf1ff; color: #294b82;
            border-radius: 10px; padding: 16px;
        }
    )");
}
