#include "writerpage.h"

#include <QAction>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>
#include <QTextEdit>
#include <QTextDocument>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>

WriterPage::WriterPage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 24, 28, 24);
    layout->setSpacing(12);

    auto *header = new QHBoxLayout;
    auto *heading = new QLabel("Nexus Writer", this);
    heading->setObjectName("writerHeading");
    header->addWidget(heading);
    header->addStretch();

    auto *newButton = new QPushButton("Nouveau", this);
    auto *openButton = new QPushButton("Ouvrir", this);
    auto *saveButton = new QPushButton("Enregistrer", this);
    newButton->setObjectName("writerSecondary");
    openButton->setObjectName("writerSecondary");
    saveButton->setObjectName("writerPrimary");
    header->addWidget(newButton);
    header->addWidget(openButton);
    header->addWidget(saveButton);
    layout->addLayout(header);

    auto *notice = new QLabel(
        "Version de démarrage : édition et enregistrement des fichiers .html et .txt. "
        "La compatibilité Word (.docx) sera ajoutée avec le moteur de documents.",
        this);
    notice->setObjectName("writerNotice");
    notice->setWordWrap(true);
    layout->addWidget(notice);

    auto *toolbar = new QToolBar("Mise en forme", this);
    toolbar->setObjectName("writerToolbar");
    toolbar->setMovable(false);

    auto *fontFamily = new QFontComboBox(toolbar);
    fontFamily->setMaximumWidth(190);
    fontFamily->setCurrentFont(QFont("Arial"));
    toolbar->addWidget(fontFamily);

    auto *fontSize = new QSpinBox(toolbar);
    fontSize->setRange(8, 72);
    fontSize->setValue(12);
    fontSize->setSuffix(" pt");
    fontSize->setMaximumWidth(82);
    toolbar->addWidget(fontSize);

    toolbar->addSeparator();

    auto *boldAction = toolbar->addAction("Gras");
    boldAction->setCheckable(true);
    boldAction->setToolTip("Mettre en gras");

    auto *italicAction = toolbar->addAction("Italique");
    italicAction->setCheckable(true);
    italicAction->setToolTip("Mettre en italique");

    auto *underlineAction = toolbar->addAction("Souligné");
    underlineAction->setCheckable(true);
    underlineAction->setToolTip("Souligner");

    toolbar->addSeparator();

    auto *leftAction = toolbar->addAction("Gauche");
    auto *centerAction = toolbar->addAction("Centrer");
    auto *rightAction = toolbar->addAction("Droite");
    auto *justifyAction = toolbar->addAction("Justifier");

    layout->addWidget(toolbar);

    editor_ = new QTextEdit(this);
    editor_->setObjectName("writerEditor");
    editor_->setAcceptRichText(true);
    editor_->setPlaceholderText("Commencez à écrire votre document…");
    editor_->setFont(QFont("Arial", 12));
    layout->addWidget(editor_, 1);

    auto *footer = new QHBoxLayout;
    documentLabel_ = new QLabel("Nouveau document", this);
    wordCountLabel_ = new QLabel("0 mot", this);
    footer->addWidget(documentLabel_);
    footer->addStretch();
    footer->addWidget(wordCountLabel_);
    layout->addLayout(footer);

    connect(newButton, &QPushButton::clicked, this, &WriterPage::newDocument);
    connect(openButton, &QPushButton::clicked, this, &WriterPage::openDocument);
    connect(saveButton, &QPushButton::clicked, this, &WriterPage::saveDocument);

    connect(fontFamily, &QFontComboBox::currentFontChanged,
            editor_, &QTextEdit::setCurrentFont);
    connect(fontSize, qOverload<int>(&QSpinBox::valueChanged),
            editor_, [this](int size) { editor_->setFontPointSize(size); });

    connect(boldAction, &QAction::toggled, editor_, [this](bool enabled) {
        editor_->setFontWeight(enabled ? QFont::Bold : QFont::Normal);
    });
    connect(italicAction, &QAction::toggled, editor_, &QTextEdit::setFontItalic);
    connect(underlineAction, &QAction::toggled, editor_, &QTextEdit::setFontUnderline);

    connect(leftAction, &QAction::triggered, editor_, [this]() {
        editor_->setAlignment(Qt::AlignLeft);
    });
    connect(centerAction, &QAction::triggered, editor_, [this]() {
        editor_->setAlignment(Qt::AlignHCenter);
    });
    connect(rightAction, &QAction::triggered, editor_, [this]() {
        editor_->setAlignment(Qt::AlignRight);
    });
    connect(justifyAction, &QAction::triggered, editor_, [this]() {
        editor_->setAlignment(Qt::AlignJustify);
    });

    connect(editor_, &QTextEdit::textChanged, this, &WriterPage::updateDocumentInfo);
    connect(editor_->document(), &QTextDocument::modificationChanged,
            this, [this](bool modified) {
                const QString name = currentFile_.isEmpty()
                    ? "Nouveau document"
                    : QFileInfo(currentFile_).fileName();
                documentLabel_->setText(modified ? name + " •" : name);
            });

    setStyleSheet(R"(
        QLabel#writerHeading { font-size: 27px; font-weight: 750; color: #172033; }
        QLabel#writerNotice {
            background: #eaf1ff; color: #294b82;
            border: 1px solid #d5e2ff; border-radius: 8px; padding: 10px 12px;
        }
        QToolBar#writerToolbar {
            background: #ffffff; border: 1px solid #e1e6ef;
            border-radius: 8px; spacing: 5px; padding: 5px;
        }
        QTextEdit#writerEditor {
            background: #ffffff; color: #172033;
            border: 1px solid #dce2ec; border-radius: 8px;
            padding: 22px; selection-background-color: #b9ccff;
        }
        QPushButton#writerSecondary, QPushButton#writerPrimary {
            border: 0; border-radius: 7px; padding: 9px 15px;
        }
        QPushButton#writerSecondary { background: #e9edf5; color: #263449; }
        QPushButton#writerPrimary { background: #315be8; color: white; }
    )");

    updateDocumentInfo();
}

bool WriterPage::maybeSave()
{
    if (!editor_->document()->isModified())
        return true;

    const auto answer = QMessageBox::question(
        this, "Document modifié",
        "Enregistrer les modifications avant de continuer ?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (answer == QMessageBox::Save) {
        saveDocument();
        return !editor_->document()->isModified();
    }
    return answer == QMessageBox::Discard;
}

void WriterPage::newDocument()
{
    if (!maybeSave())
        return;

    editor_->clear();
    currentFile_.clear();
    editor_->document()->setModified(false);
    updateDocumentInfo();
}

void WriterPage::openDocument()
{
    if (!maybeSave())
        return;

    const QString path = QFileDialog::getOpenFileName(
        this, "Ouvrir un document", QString(),
        "Documents Nexus Writer (*.html *.htm *.txt *.md);;"
        "Documents HTML (*.html *.htm);;Fichiers texte (*.txt *.md)");

    if (path.isEmpty())
        return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ouverture impossible",
                              "Le fichier n'a pas pu être ouvert :\n" + file.errorString());
        return;
    }

    QTextStream stream(&file);
    const QString contents = stream.readAll();
    const QString suffix = QFileInfo(path).suffix().toLower();

    if (suffix == "html" || suffix == "htm")
        editor_->setHtml(contents);
    else
        editor_->setPlainText(contents);

    currentFile_ = path;
    editor_->document()->setModified(false);
    updateDocumentInfo();
}

void WriterPage::saveDocument()
{
    if (currentFile_.isEmpty()) {
        saveDocumentAs();
        return;
    }
    saveToFile(currentFile_);
}

void WriterPage::saveDocumentAs()
{
    const QString path = QFileDialog::getSaveFileName(
        this, "Enregistrer le document", currentFile_.isEmpty()
            ? "Document sans titre.html" : currentFile_,
        "Document HTML (*.html);;Fichier texte (*.txt);;Markdown (*.md)");

    if (path.isEmpty())
        return;

    QString finalPath = path;
    if (QFileInfo(finalPath).suffix().isEmpty())
        finalPath += ".html";

    if (saveToFile(finalPath)) {
        currentFile_ = finalPath;
        updateDocumentInfo();
    }
}

bool WriterPage::saveToFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Enregistrement impossible",
                              "Le fichier n'a pas pu être enregistré :\n" + file.errorString());
        return false;
    }

    QTextStream stream(&file);
    const QString suffix = QFileInfo(path).suffix().toLower();
    if (suffix == "html" || suffix == "htm")
        stream << editor_->toHtml();
    else
        stream << editor_->toPlainText();

    file.close();
    editor_->document()->setModified(false);
    updateDocumentInfo();
    return true;
}

void WriterPage::updateDocumentInfo()
{
    const int words = editor_->toPlainText()
        .split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).size();
    wordCountLabel_->setText(QString("%1 mot%2").arg(words).arg(words == 1 ? "" : "s"));
    if (currentFile_.isEmpty() && !editor_->document()->isModified())
        documentLabel_->setText("Nouveau document");
}
