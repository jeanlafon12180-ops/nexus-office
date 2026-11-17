#include "writerpage.h"

#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
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
#include <QSettings>
#include <QPageSize>
#include <QPageLayout>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMenuBar>
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
#include <QTimer>
#include <QStandardPaths>
#include <QDir>
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
    setObjectName("writerWorkspace");
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // LibreOffice-inspired menu bar
    auto *menuBar = new QMenuBar(this);
    menuBar->setObjectName("writerMenuBar");
    auto *fileMenu = menuBar->addMenu("Fichier");
    auto *editMenu = menuBar->addMenu("Édition");
    auto *viewMenu = menuBar->addMenu("Affichage");
    auto *insertMenu = menuBar->addMenu("Insertion");
    auto *formatMenu = menuBar->addMenu("Format");
    auto *stylesMenu = menuBar->addMenu("Styles");
    auto *tableMenu = menuBar->addMenu("Tableau");
    auto *toolsMenu = menuBar->addMenu("Outils");
    auto *windowMenu = menuBar->addMenu("Fenêtre");
    auto *helpMenu = menuBar->addMenu("Aide");
    layout->addWidget(menuBar);

    auto *fileNew = fileMenu->addAction("Nouveau");
    auto *fileOpen = fileMenu->addAction("Ouvrir…");
    auto *fileSave = fileMenu->addAction("Enregistrer");
    fileMenu->addSeparator();
    auto *filePdf = fileMenu->addAction("Exporter en PDF…");
    auto *filePrint = fileMenu->addAction("Imprimer…");
    auto *editUndo = editMenu->addAction("Annuler");
    auto *editRedo = editMenu->addAction("Rétablir");
    auto *editFind = editMenu->addAction("Rechercher…");
    auto *insertTable = insertMenu->addAction("Tableau…");
    auto *insertImage = insertMenu->addAction("Image…");
    auto *toolSpell = toolsMenu->addAction("Vérification orthographique");
    auto *helpAbout = helpMenu->addAction("À propos de Nexus Writer");
    Q_UNUSED(viewMenu);
    Q_UNUSED(formatMenu);
    Q_UNUSED(stylesMenu);
    Q_UNUSED(tableMenu);
    Q_UNUSED(windowMenu);
    Q_UNUSED(helpAbout);

    // Compact file / editing toolbar
    auto *fileToolbar = new QToolBar("Standard", this);
    fileToolbar->setObjectName("writerStandardToolbar");
    fileToolbar->setMovable(false);
    fileToolbar->setIconSize(QSize(18, 18));
    fileToolbar->addAction(fileNew);
    fileToolbar->addAction(fileOpen);
    fileToolbar->addAction(fileSave);
    fileToolbar->addSeparator();
    fileToolbar->addAction(filePdf);
    fileToolbar->addAction(filePrint);
    fileToolbar->addSeparator();
    fileToolbar->addAction(editUndo);
    fileToolbar->addAction(editRedo);
    fileToolbar->addAction(editFind);
    layout->addWidget(fileToolbar);

    // Formatting toolbar
    auto *toolbar = new QToolBar("Formatage", this);
    toolbar->setObjectName("writerToolbar");
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(18, 18));

    auto *paragraphStyle = new QComboBox(toolbar);
    paragraphStyle->addItems({"Style par défaut", "Titre 1", "Titre 2", "Titre 3"});
    paragraphStyle->setMinimumWidth(170);
    toolbar->addWidget(paragraphStyle);

    auto *fontFamily = new QFontComboBox(toolbar);
    fontFamily->setMaximumWidth(190);
    fontFamily->setCurrentFont(QFont("Liberation Serif"));
    toolbar->addWidget(fontFamily);

    auto *fontSize = new QSpinBox(toolbar);
    fontSize->setRange(8, 72);
    fontSize->setValue(12);
    fontSize->setSuffix(" pt");
    fontSize->setMaximumWidth(78);
    toolbar->addWidget(fontSize);
    toolbar->addSeparator();

    auto *boldAction = toolbar->addAction("G");
    boldAction->setCheckable(true);
    boldAction->setToolTip("Gras");
    auto *italicAction = toolbar->addAction("I");
    italicAction->setCheckable(true);
    italicAction->setToolTip("Italique");
    auto *underlineAction = toolbar->addAction("S");
    underlineAction->setCheckable(true);
    underlineAction->setToolTip("Souligné");
    toolbar->addSeparator();
    auto *leftAction = toolbar->addAction("Aligner à gauche");
    auto *centerAction = toolbar->addAction("Centrer");
    auto *rightAction = toolbar->addAction("Aligner à droite");
    auto *justifyAction = toolbar->addAction("Justifier");
    toolbar->addSeparator();
    auto *bulletAction = toolbar->addAction("Liste •");
    auto *numberAction = toolbar->addAction("Liste 1.");
    auto *tableAction = toolbar->addAction("Tableau");
    auto *imageAction = toolbar->addAction("Image");
    layout->addWidget(toolbar);

    // Grey workspace with a centered white A4-like page
    auto *workspace = new QWidget(this);
    workspace->setObjectName("writerCanvas");
    auto *workspaceLayout = new QHBoxLayout(workspace);
    workspaceLayout->setContentsMargins(24, 24, 24, 24);
    workspaceLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    editor_ = new QTextEdit(workspace);
    editor_->setObjectName("writerEditor");
    editor_->setAcceptRichText(true);
    editor_->setPlaceholderText("Commencez à écrire votre document…");
    editor_->setFont(QFont("Liberation Serif", 12));
    editor_->setMinimumWidth(650);
    editor_->setMaximumWidth(850);
    editor_->setMinimumHeight(1050);
    editor_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    editor_->document()->setDocumentMargin(72);
    workspaceLayout->addWidget(editor_);
    layout->addWidget(workspace, 1);

    // LibreOffice-style status bar
    auto *statusBar = new QWidget(this);
    statusBar->setObjectName("writerStatusBar");
    auto *statusLayout = new QHBoxLayout(statusBar);
    statusLayout->setContentsMargins(10, 4, 10, 4);
    documentLabel_ = new QLabel("Page 1 de 1", statusBar);
    wordCountLabel_ = new QLabel("0 mot, 0 caractère", statusBar);
    statusLayout->addWidget(documentLabel_);
    statusLayout->addSpacing(24);
    statusLayout->addWidget(wordCountLabel_);
    statusLayout->addStretch();
    auto *languageLabel = new QLabel("Français (France)", statusBar);
    auto *zoomLabel = new QLabel("100 %", statusBar);
    statusLayout->addWidget(languageLabel);
    statusLayout->addSpacing(18);
    statusLayout->addWidget(zoomLabel);
    layout->addWidget(statusBar);

    connect(fileNew, &QAction::triggered, this, &WriterPage::newDocument);
    connect(fileOpen, &QAction::triggered, this, &WriterPage::openDocument);
    connect(fileSave, &QAction::triggered, this, &WriterPage::saveDocument);
    connect(filePdf, &QAction::triggered, this, &WriterPage::exportPdf);
    connect(filePrint, &QAction::triggered, this, &WriterPage::printDocument);
    connect(editUndo, &QAction::triggered, editor_, &QTextEdit::undo);
    connect(editRedo, &QAction::triggered, editor_, &QTextEdit::redo);
    connect(editFind, &QAction::triggered, this, &WriterPage::findText);
    connect(toolSpell, &QAction::triggered, this, &WriterPage::checkSpelling);
    connect(insertTable, &QAction::triggered, this, &WriterPage::insertTable);
    connect(insertImage, &QAction::triggered, this, &WriterPage::insertImage);
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
    connect(leftAction, &QAction::triggered, editor_, [this]() { editor_->setAlignment(Qt::AlignLeft); });
    connect(centerAction, &QAction::triggered, editor_, [this]() { editor_->setAlignment(Qt::AlignHCenter); });
    connect(rightAction, &QAction::triggered, editor_, [this]() { editor_->setAlignment(Qt::AlignRight); });
    connect(justifyAction, &QAction::triggered, editor_, [this]() { editor_->setAlignment(Qt::AlignJustify); });

    connect(newButton, &QPushButton::clicked, this, &WriterPage::newDocument);
    connect(openButton, &QPushButton::clicked, this, &WriterPage::openDocument);
    connect(saveButton, &QPushButton::clicked, this, &WriterPage::saveDocument);
    connect(pdfButton, &QPushButton::clicked, this, &WriterPage::exportPdf);
    connect(printButton, &QPushButton::clicked, this, &WriterPage::printDocument);
    connect(spellButton, &QPushButton::clicked, this, &WriterPage::checkSpelling);
    connect(templateButton, &QPushButton::clicked, this, &WriterPage::showTemplates);
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
    recoveryFile_ = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/writer-recovery.html";
    QDir().mkpath(QFileInfo(recoveryFile_).absolutePath());
    recoveryTimer_ = new QTimer(this);
    recoveryTimer_->setInterval(15000);
    connect(recoveryTimer_, &QTimer::timeout, this, &WriterPage::writeRecoveryCopy);
    recoveryTimer_->start();
    QFile recovery(recoveryFile_);
    if (recovery.exists() && recovery.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QString recovered = QString::fromUtf8(recovery.readAll());
        if (!recovered.isEmpty() && QMessageBox::question(this, "Récupération", "Une version de récupération de Writer a été trouvée. Voulez-vous la restaurer ?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            editor_->setHtml(recovered);
            editor_->document()->setModified(true);
            updateDocumentInfo();
        } else {
            clearRecoveryCopy();
        }
    }
    connect(editor_->document(), &QTextDocument::modificationChanged,
            this, [this](bool modified) {
                const QString name = currentFile_.isEmpty()
                    ? "Nouveau document"
                    : QFileInfo(currentFile_).fileName();
                documentLabel_->setText(modified ? name + " •" : name);
            });

    setStyleSheet(R"(
        QWidget#writerWorkspace { background: #202020; color: #eeeeee; }
        QMenuBar#writerMenuBar { background: #242424; color: #f1f1f1; border: 0; padding: 3px 5px; }
        QMenuBar#writerMenuBar::item { padding: 5px 9px; background: transparent; }
        QMenuBar#writerMenuBar::item:selected { background: #3b3b3b; }
        QMenu { background: #292929; color: #f1f1f1; border: 1px solid #555555; }
        QMenu::item:selected { background: #315b9b; }
        QToolBar#writerStandardToolbar, QToolBar#writerToolbar {
            background: #292929; color: #f2f2f2; border: 0; spacing: 5px; padding: 4px 6px;
        }
        QToolBar#writerStandardToolbar QToolButton, QToolBar#writerToolbar QToolButton {
            color: #f2f2f2; background: transparent; border: 0; padding: 5px 7px; border-radius: 3px;
        }
        QToolBar#writerStandardToolbar QToolButton:hover, QToolBar#writerToolbar QToolButton:hover {
            background: #454545;
        }
        QToolBar#writerToolbar QComboBox, QToolBar#writerToolbar QSpinBox {
            background: #f7f7f7; color: #202020; border: 1px solid #777777; padding: 3px;
        }
        QWidget#writerCanvas { background: #202020; }
        QTextEdit#writerEditor {
            background: #ffffff; color: #111111; border: 1px solid #bdbdbd;
            padding: 0px; selection-background-color: #b7d2ff;
        }
        QWidget#writerStatusBar {
            background: #252525; color: #eeeeee; border-top: 1px solid #444444;
        }
        QWidget#writerStatusBar QLabel { color: #eeeeee; padding: 2px 5px; }
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
    clearRecoveryCopy();
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
    clearRecoveryCopy();
    emit documentOpened(path, "Writer");
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
    emit documentOpened(path, "Writer");
    editor_->document()->setModified(false);
    clearRecoveryCopy();
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
    QSettings preferences;
    const QString paper=preferences.value("document/paperSize", "A4").toString();
    printer.setPageSize(QPageSize(paper=="A3" ? QPageSize::A3 : paper=="Letter" ? QPageSize::Letter : QPageSize::A4));
    printer.setPageOrientation(preferences.value("document/orientation", "Portrait").toString()=="Paysage" ? QPageLayout::Landscape : QPageLayout::Portrait);
    QPageSetupDialog pageSetup(&printer, this);
    pageSetup.setWindowTitle("Mise en page du PDF");
    if (pageSetup.exec() != QDialog::Accepted)
        return;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    editor_->document()->print(&printer);
}

void WriterPage::printDocument()
{
    QPrinter printer(QPrinter::HighResolution);
    QSettings preferences;
    const QString paper=preferences.value("document/paperSize", "A4").toString();
    printer.setPageSize(QPageSize(paper=="A3" ? QPageSize::A3 : paper=="Letter" ? QPageSize::Letter : QPageSize::A4));
    printer.setPageOrientation(preferences.value("document/orientation", "Portrait").toString()=="Paysage" ? QPageLayout::Landscape : QPageLayout::Portrait);
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


void WriterPage::checkSpelling()
{
    const QString hunspell = QStandardPaths::findExecutable("hunspell");
    if (hunspell.isEmpty()) {
        QMessageBox::information(this, "Correcteur orthographique",
            "Hunspell n’est pas installé. Installez Hunspell avec le dictionnaire français pour activer cette fonction.");
        return;
    }

    QProcess process;
    process.start(hunspell, {"-l", "-d", "fr_FR"});
    if (!process.waitForStarted(5000)) {
        QMessageBox::warning(this, "Correcteur orthographique", "Impossible de démarrer Hunspell.");
        return;
    }
    process.write(editor_->toPlainText().toUtf8());
    process.closeWriteChannel();
    if (!process.waitForFinished(15000)) {
        process.kill();
        process.waitForFinished();
        QMessageBox::warning(this, "Correcteur orthographique", "La vérification a dépassé le délai autorisé.");
        return;
    }
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        const QString details = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
        QMessageBox::warning(this, "Correcteur orthographique",
            details.isEmpty() ? "Hunspell ou le dictionnaire français n’est pas disponible." : details);
        return;
    }

    const QStringList misspelled = QString::fromUtf8(process.readAllStandardOutput())
        .split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    if (misspelled.isEmpty()) {
        QMessageBox::information(this, "Correcteur orthographique", "Aucune faute détectée avec le dictionnaire français.");
        return;
    }
    const QString report = misspelled.mid(0, 80).join("\n");
    const QString suffix = misspelled.size() > 80
        ? QString("\n… et %1 autre(s) mot(s).").arg(misspelled.size() - 80)
        : QString();
    QMessageBox::information(this, "Mots à vérifier",
        report + suffix + "\n\nCette liste est une aide : vérifiez le contexte avant toute correction.");
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

void WriterPage::writeRecoveryCopy()
{
    if (!editor_ || !editor_->document()->isModified()) return;
    QSaveFile file(recoveryFile_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    file.write(editor_->toHtml().toUtf8());
    file.commit();
}

void WriterPage::clearRecoveryCopy()
{
    if (!recoveryFile_.isEmpty()) QFile::remove(recoveryFile_);
}

void WriterPage::showTemplates()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Galerie de modèles Writer");
    auto *layout = new QVBoxLayout(&dialog);
    auto *choices = new QComboBox(&dialog);
    choices->addItems({"Lettre professionnelle", "CV simple", "Rapport scolaire", "Facture simple", "Page vierge"});
    layout->addWidget(new QLabel("Choisissez un modèle :", &dialog));
    layout->addWidget(choices);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    if (!editor_->document()->isEmpty() && QMessageBox::question(this, "Remplacer le contenu", "Le modèle remplacera le contenu actuel. Continuer ?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    const QString choice = choices->currentText();
    if (choice == "Lettre professionnelle")
        editor_->setHtml("<h1>Nom et prénom</h1><p>Adresse<br>Code postal – Ville</p><p>Ville, le date</p><p><b>Objet :</b> Objet de la lettre</p><p>Madame, Monsieur,</p><p>Présentez votre demande ici.</p><p>Veuillez agréer, Madame, Monsieur, l’expression de mes salutations distinguées.</p><p>Signature</p>");
    else if (choice == "CV simple")
        editor_->setHtml("<h1>Prénom NOM</h1><h2>Profil</h2><p>Présentez votre parcours en quelques lignes.</p><h2>Expériences</h2><p>Poste — Organisation — Dates</p><h2>Formation</h2><p>Diplôme — Établissement — Année</p><h2>Compétences</h2><p>Compétence 1 · Compétence 2</p><h2>Langues</h2><p>Langue — Niveau</p>");
    else if (choice == "Rapport scolaire")
        editor_->setHtml("<h1>Titre du rapport</h1><p><b>Nom :</b> … &nbsp; <b>Classe :</b> …</p><h2>Introduction</h2><p>Présentez le sujet et la problématique.</p><h2>Développement</h2><p>Partie 1…</p><h2>Conclusion</h2><p>Résumez les idées principales.</p><h2>Sources</h2><p>Liste des sources consultées.</p>");
    else if (choice == "Facture simple")
        editor_->setHtml("<h1>FACTURE</h1><p><b>Émetteur :</b> Nom, adresse</p><p><b>Client :</b> Nom, adresse</p><p><b>Numéro :</b> … &nbsp; <b>Date :</b> …</p><table border='1' cellpadding='5'><tr><th>Désignation</th><th>Quantité</th><th>Prix unitaire</th><th>Total</th></tr><tr><td>Prestation</td><td>1</td><td>0,00 €</td><td>0,00 €</td></tr></table><p><b>Total à payer :</b> 0,00 €</p>");
    else editor_->clear();
    currentFile_.clear();
    editor_->document()->setModified(true);
    updateDocumentInfo();
}
