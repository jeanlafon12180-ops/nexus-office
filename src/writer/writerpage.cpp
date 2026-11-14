#include "writerpage.h"

#include <QAction>
#include <QFile>
#include <QSaveFile>
#include <QInputDialog>
#include <QLineEdit>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextListFormat>
#include <QImageReader>
#include <QUrl>
#include <QComboBox>
#include <QKeySequence>
#include <QPrinter>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QDir>
#include <QRegularExpression>
#include <QSpinBox>
#include <QTextEdit>
#include <QTextDocument>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>


namespace {
QString findSoffice()
{
    QString exe = QStandardPaths::findExecutable("soffice");
#ifdef Q_OS_WIN
    if (exe.isEmpty() && QFileInfo::exists("C:/Program Files/LibreOffice/program/soffice.exe"))
        exe = "C:/Program Files/LibreOffice/program/soffice.exe";
    if (exe.isEmpty() && QFileInfo::exists("C:/Program Files (x86)/LibreOffice/program/soffice.exe"))
        exe = "C:/Program Files (x86)/LibreOffice/program/soffice.exe";
#elif defined(Q_OS_MACOS)
    if (exe.isEmpty() && QFileInfo::exists("/Applications/LibreOffice.app/Contents/MacOS/soffice"))
        exe = "/Applications/LibreOffice.app/Contents/MacOS/soffice";
#endif
    return exe;
}
bool convertWithLibreOffice(const QStringList &args, QString *error)
{
    const QString exe = findSoffice();
    if (exe.isEmpty()) {
        *error = "LibreOffice est introuvable. Installez-le pour utiliser les fichiers DOCX.";
        return false;
    }
    QTemporaryDir profile;
    if (!profile.isValid()) {
        *error = "Impossible de préparer le profil temporaire de LibreOffice.";
        return false;
    }
    QStringList isolatedArgs = args;
    isolatedArgs.prepend("-env:UserInstallation=" + QUrl::fromLocalFile(profile.path()).toString());
    QProcess p;
    p.start(exe, isolatedArgs);
    if (!p.waitForStarted(10000) || !p.waitForFinished(120000) ||
        p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0) {
        if (p.state() != QProcess::NotRunning) { p.kill(); p.waitForFinished(); }
        *error = QString::fromLocal8Bit(p.readAllStandardError()).trimmed();
        if (error->isEmpty()) *error = "La conversion LibreOffice a échoué.";
        return false;
    }
    return true;
}
}

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
    auto *pdfButton = new QPushButton("Exporter PDF", this);
    auto *printButton = new QPushButton("Imprimer", this);
    newButton->setObjectName("writerSecondary");
    openButton->setObjectName("writerSecondary");
    saveButton->setObjectName("writerPrimary");
    header->addWidget(newButton);
    header->addWidget(openButton);
    header->addWidget(saveButton);
    header->addWidget(pdfButton);
    header->addWidget(printButton);
    layout->addLayout(header);

    auto *notice = new QLabel(
        "Rédigez et mettez en forme vos documents. Enregistrement HTML, TXT, Markdown et DOCX (via LibreOffice).",
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
    auto *undoAction = toolbar->addAction("Annuler");
    auto *redoAction = toolbar->addAction("Rétablir");
    undoAction->setShortcut(QKeySequence::Undo);
    redoAction->setShortcut(QKeySequence::Redo);
    toolbar->addSeparator();
    auto *paragraphStyle = new QComboBox(toolbar);
    paragraphStyle->addItems({"Corps de texte", "Titre 1", "Titre 2", "Titre 3"});
    paragraphStyle->setToolTip("Style du paragraphe");
    toolbar->addWidget(paragraphStyle);
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
    auto *bulletAction = toolbar->addAction("Liste •");
    auto *numberAction = toolbar->addAction("Liste 1.");
    auto *tableAction = toolbar->addAction("Insérer tableau");
    auto *imageAction = toolbar->addAction("Insérer image");

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
    connect(pdfButton, &QPushButton::clicked, this, &WriterPage::exportPdf);
    connect(printButton, &QPushButton::clicked, this, &WriterPage::printDocument);
    connect(undoAction, &QAction::triggered, editor_, &QTextEdit::undo);
    connect(redoAction, &QAction::triggered, editor_, &QTextEdit::redo);
    auto *findAction = toolbar->addAction("Rechercher");
    findAction->setShortcut(QKeySequence::Find);
    connect(findAction, &QAction::triggered, this, &WriterPage::findText);
    connect(paragraphStyle, qOverload<int>(&QComboBox::currentIndexChanged), this, &WriterPage::applyParagraphStyle);
    connect(bulletAction, &QAction::triggered, this, [this]() { insertList(false); });
    connect(numberAction, &QAction::triggered, this, [this]() { insertList(true); });
    connect(tableAction, &QAction::triggered, this, &WriterPage::insertTable);
    connect(imageAction, &QAction::triggered, this, &WriterPage::insertImage);

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
        QLabel { background: transparent; border: 0; }
        QLabel { background: transparent; border: 0; }
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
    if (!maybeSave()) return;
    const QString path = QFileDialog::getOpenFileName(
        this, "Ouvrir un document", QString(),
        "Documents (*.docx *.html *.htm *.txt *.md);;Documents Word (*.docx);;Documents HTML (*.html *.htm);;Fichiers texte (*.txt *.md)");
    if (path.isEmpty()) return;
    const QString suffix = QFileInfo(path).suffix().toLower();
    if (suffix == "docx") {
        QTemporaryDir temp;
        QString error;
        if (!temp.isValid() || !convertWithLibreOffice(
                {"--headless", "--convert-to", "html", "--outdir", temp.path(), path}, &error)) {
            QMessageBox::critical(this, "Ouverture DOCX impossible",
                error.isEmpty() ? "Impossible de créer un dossier temporaire." : error);
            return;
        }
        QFile converted(temp.filePath(QFileInfo(path).completeBaseName() + ".html"));
        if (!converted.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Ouverture impossible", "LibreOffice n’a pas généré le document converti.");
            return;
        }
        QTextStream stream(&converted);
        editor_->setHtml(stream.readAll());
    } else {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Ouverture impossible", "Le fichier n’a pas pu être ouvert :\
" + file.errorString());
            return;
        }
        QTextStream stream(&file);
        const QString contents = stream.readAll();
        if (suffix == "html" || suffix == "htm") editor_->setHtml(contents);
        else if (suffix == "md" || suffix == "markdown") editor_->document()->setMarkdown(contents);
        else editor_->setPlainText(contents);
    }
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
        this, "Enregistrer le document",
        currentFile_.isEmpty() ? "Document sans titre.html" : currentFile_,
        "Document Word (*.docx);;Document HTML (*.html);;Fichier texte (*.txt);;Markdown (*.md)");

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
    const QString suffix = QFileInfo(path).suffix().toLower();
    if (suffix == "docx") {
        QTemporaryDir temp;
        if (!temp.isValid()) {
            QMessageBox::critical(this, "Enregistrement impossible", "Impossible de créer un dossier temporaire.");
            return false;
        }
        const QString htmlPath = temp.filePath("NexusDocument.html");
        QFile html(htmlPath);
        if (!html.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Enregistrement impossible", "Impossible de préparer le document.");
            return false;
        }
        QTextStream stream(&html);
        stream << editor_->toHtml();
        html.close();
        QString error;
        if (!convertWithLibreOffice({"--headless", "--convert-to", "docx:Office Open XML Text", "--outdir", temp.path(), htmlPath}, &error)) {
            QMessageBox::critical(this, "Enregistrement DOCX impossible", error);
            return false;
        }
        const QString generated = temp.filePath("NexusDocument.docx");
        if (!QFileInfo::exists(generated)) {
            QMessageBox::critical(this, "Enregistrement impossible", "LibreOffice n’a pas généré le fichier DOCX.");
            return false;
        }
        QFile source(generated);
        QSaveFile destination(path);
        if (!source.open(QIODevice::ReadOnly) || !destination.open(QIODevice::WriteOnly) ||
            destination.write(source.readAll()) < 0 || !destination.commit()) {
            QMessageBox::critical(this, "Enregistrement impossible", "Impossible d’enregistrer le fichier DOCX à cet emplacement.");
            return false;
        }
    } else {
        QSaveFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Enregistrement impossible", "Le fichier n’a pas pu être enregistré :\n" + file.errorString());
            return false;
        }
        QTextStream stream(&file);
        if (suffix == "html" || suffix == "htm") stream << editor_->toHtml();
        else if (suffix == "md" || suffix == "markdown") stream << editor_->document()->toMarkdown();
        else stream << editor_->toPlainText();
        if (!file.commit()) {
            QMessageBox::critical(this, "Enregistrement impossible", "Impossible de finaliser l’enregistrement :\n" + file.errorString());
            return false;
        }
    }
    editor_->document()->setModified(false);
    updateDocumentInfo();
    return true;
}

void WriterPage::findText()
{
    bool accepted = false;
    const QString query = QInputDialog::getText(this, "Rechercher", "Texte à rechercher :", QLineEdit::Normal, QString(), &accepted);
    if (!accepted || query.isEmpty()) return;
    if (!editor_->find(query)) {
        QTextCursor cursor = editor_->textCursor();
        cursor.movePosition(QTextCursor::Start);
        editor_->setTextCursor(cursor);
        if (!editor_->find(query)) QMessageBox::information(this, "Recherche", "Aucune occurrence trouvée.");
    }
}

void WriterPage::exportPdf()
{
    QString path = QFileDialog::getSaveFileName(this, "Exporter en PDF", "Document.pdf", "Fichier PDF (*.pdf)");
    if (path.isEmpty()) return;
    if (QFileInfo(path).suffix().isEmpty()) path += ".pdf";
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    editor_->document()->print(&printer);
}

void WriterPage::printDocument()
{
    QPrinter printer(QPrinter::HighResolution);
    QPageSetupDialog pageSetup(&printer, this);
    pageSetup.setWindowTitle("Mise en page et impression");
    if (pageSetup.exec() != QDialog::Accepted) return;
    QPrintDialog printDialog(&printer, this);
    printDialog.setWindowTitle("Imprimer le document");
    if (printDialog.exec() == QDialog::Accepted)
        editor_->document()->print(&printer);
}

void WriterPage::applyParagraphStyle(int index)
{
    QTextCursor cursor = editor_->textCursor();
    QTextBlockFormat block = cursor.blockFormat();
    QTextCharFormat character;
    switch (index) {
    case 1:
        block.setHeadingLevel(1);
        character.setFontPointSize(22);
        character.setFontWeight(QFont::Bold);
        break;
    case 2:
        block.setHeadingLevel(2);
        character.setFontPointSize(18);
        character.setFontWeight(QFont::Bold);
        break;
    case 3:
        block.setHeadingLevel(3);
        character.setFontPointSize(14);
        character.setFontWeight(QFont::DemiBold);
        break;
    default:
        block.setHeadingLevel(0);
        character.setFontPointSize(12);
        character.setFontWeight(QFont::Normal);
        break;
    }
    cursor.setBlockFormat(block);
    cursor.mergeCharFormat(character);
    editor_->setTextCursor(cursor);
    editor_->setFocus();
}

void WriterPage::insertList(bool numbered)
{
    QTextCursor cursor = editor_->textCursor();
    QTextListFormat format;
    format.setStyle(numbered ? QTextListFormat::ListDecimal : QTextListFormat::ListDisc);
    cursor.createList(format);
    editor_->setTextCursor(cursor);
    editor_->setFocus();
}

void WriterPage::insertTable()
{
    bool rowsOk = false;
    const int rows = QInputDialog::getInt(this, "Insérer un tableau", "Nombre de lignes :", 3, 1, 50, 1, &rowsOk);
    if (!rowsOk) return;
    bool columnsOk = false;
    const int columns = QInputDialog::getInt(this, "Insérer un tableau", "Nombre de colonnes :", 3, 1, 20, 1, &columnsOk);
    if (!columnsOk) return;
    QTextCursor cursor = editor_->textCursor();
    cursor.insertTable(rows, columns);
    editor_->setTextCursor(cursor);
    editor_->setFocus();
}

void WriterPage::insertImage()
{
    const QString path = QFileDialog::getOpenFileName(this, "Insérer une image", QString(),
        "Images (*.png *.jpg *.jpeg *.bmp *.gif *.webp)");
    if (path.isEmpty()) return;
    QImageReader reader(path);
    const QImage image = reader.read();
    if (image.isNull()) {
        QMessageBox::warning(this, "Image impossible à ouvrir", reader.errorString());
        return;
    }
    QTextImageFormat format;
    format.setName(path);
    const QSize maxSize(640, 480);
    const QSize scaled = image.size().scaled(maxSize, Qt::KeepAspectRatio);
    format.setWidth(scaled.width());
    format.setHeight(scaled.height());
    QTextCursor cursor = editor_->textCursor();
    cursor.insertImage(format);
    editor_->setTextCursor(cursor);
    editor_->setFocus();
}

void WriterPage::updateDocumentInfo()
{
    const int words = editor_->toPlainText()
        .split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).size();
    const int characters = editor_->toPlainText().size();
    wordCountLabel_->setText(QString("%1 mot%2 · %3 caractères").arg(words).arg(words == 1 ? "" : "s").arg(characters));
    if (currentFile_.isEmpty() && !editor_->document()->isModified())
        documentLabel_->setText("Nouveau document");
}
