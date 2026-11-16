#include "pdfpage.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPdfDocument>
#include <QPdfView>
#include <QPdfPageNavigator>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>

PdfPage::PdfPage(QWidget *parent) : QWidget(parent)
{
    document_ = new QPdfDocument(this);
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(22, 18, 22, 18);
    layout->setSpacing(10);

    auto *heading = new QLabel("Nexus PDF", this);
    heading->setObjectName("pdfHeading");
    layout->addWidget(heading);

    auto *toolbar = new QHBoxLayout;
    auto *open = new QPushButton("Ouvrir");
    open->setObjectName("pdfOpenButton");
    toolbar->addWidget(open);

    pageSpin_ = new QSpinBox(this);
    pageSpin_->setObjectName("pdfPageSpin");
    pageSpin_->setPrefix("Page ");
    pageSpin_->setMinimum(1);
    pageSpin_->setEnabled(false);
    pageSpin_->setMaximumWidth(110);
    toolbar->addWidget(pageSpin_);

    auto *zoomLabel = new QLabel("Zoom", this);
    toolbar->addWidget(zoomLabel);
    zoomCombo_ = new QComboBox(this);
    zoomCombo_->setObjectName("pdfZoomCombo");
    zoomCombo_->addItems({"Ajuster à la page", "Ajuster à la largeur", "50 %", "75 %", "100 %", "125 %", "150 %", "200 %"});
    zoomCombo_->setCurrentIndex(0);
    zoomCombo_->setEnabled(false);
    toolbar->addWidget(zoomCombo_);

    searchEdit_ = new QLineEdit(this);
    searchEdit_->setObjectName("pdfSearchEdit");
    searchEdit_->setPlaceholderText("Rechercher dans le PDF…");
    searchEdit_->setEnabled(false);
    toolbar->addWidget(searchEdit_, 1);
    auto *find = new QPushButton("Suivant");
    find->setObjectName("pdfFindButton");
    find->setEnabled(false);
    toolbar->addWidget(find);

    notesButton_ = new QPushButton("Commentaire");
    notesButton_->setObjectName("pdfNoteButton");
    notesButton_->setEnabled(false);
    toolbar->addWidget(notesButton_);
    auto *print = new QPushButton("Imprimer");
    print->setObjectName("pdfPrintButton");
    print->setEnabled(false);
    toolbar->addWidget(print);
    layout->addLayout(toolbar);

    fileLabel_ = new QLabel("Aucun PDF ouvert", this);
    fileLabel_->setObjectName("pdfFileLabel");
    layout->addWidget(fileLabel_);

    view_ = new QPdfView(this);
    view_->setObjectName("pdfDocumentView");
    view_->setDocument(document_);
    view_->setPageMode(QPdfView::PageMode::MultiPage);
    view_->setZoomMode(QPdfView::ZoomMode::FitInView);
    layout->addWidget(view_, 1);

    statusLabel_ = new QLabel("Ouvrez un PDF pour commencer.", this);
    statusLabel_->setObjectName("pdfStatusLabel");
    layout->addWidget(statusLabel_);

    connect(open, &QPushButton::clicked, this, &PdfPage::openPdf);
    connect(pageSpin_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int page) {
        if (document_->status() == QPdfDocument::Status::Ready)
            view_->pageNavigator()->jump(page - 1);
    });
    connect(zoomCombo_, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (index == 0) view_->setZoomMode(QPdfView::ZoomMode::FitInView);
        else if (index == 1) view_->setZoomMode(QPdfView::ZoomMode::FitToWidth);
        else {
            static const qreal factors[] = {0.5, 0.75, 1.0, 1.25, 1.5, 2.0};
            view_->setZoomMode(QPdfView::ZoomMode::Custom);
            view_->setZoomFactor(factors[index - 2]);
        }
    });
    connect(find, &QPushButton::clicked, this, &PdfPage::findNext);
    connect(searchEdit_, &QLineEdit::returnPressed, this, &PdfPage::findNext);
    connect(notesButton_, &QPushButton::clicked, this, &PdfPage::editPageNote);
    connect(print, &QPushButton::clicked, this, &PdfPage::printPdf);
    connect(document_, &QPdfDocument::statusChanged, this, [this](QPdfDocument::Status) {
        updateDocumentState();
    });
}

void PdfPage::openPdf()
{
    const QString path = QFileDialog::getOpenFileName(this, "Ouvrir un PDF", QString(), "Documents PDF (*.pdf)");
    if (path.isEmpty()) return;
    document_->close();
    const auto error = document_->load(path);
    if (error != QPdfDocument::Error::None) {
        QMessageBox::warning(this, "Nexus PDF", "Impossible de charger ce fichier PDF.");
        currentPath_.clear();
        updateDocumentState();
        return;
    }
    currentPath_ = path;
    lastSearchPage_ = -1;
    fileLabel_->setText(QFileInfo(path).fileName());
    updateDocumentState();
}

void PdfPage::updateDocumentState()
{
    const bool ready = document_->status() == QPdfDocument::Status::Ready;
    const int pages = ready ? document_->pageCount() : 0;
    pageSpin_->setEnabled(ready && pages > 0);
    pageSpin_->setRange(1, qMax(1, pages));
    zoomCombo_->setEnabled(ready);
    searchEdit_->setEnabled(ready);
    notesButton_->setEnabled(ready);
    for (auto *button : findChildren<QPushButton *>()) {
        if (button->objectName() == "pdfFindButton" || button->objectName() == "pdfPrintButton")
            button->setEnabled(ready);
    }
    if (ready) {
        statusLabel_->setText(QString("%1 page(s) — commentaires enregistrés dans un fichier annexe.").arg(pages));
    } else if (currentPath_.isEmpty()) {
        statusLabel_->setText("Ouvrez un PDF pour commencer.");
    }
}

void PdfPage::findNext()
{
    const QString query = searchEdit_->text().trimmed();
    if (query.isEmpty() || document_->status() != QPdfDocument::Status::Ready) return;
    const int count = document_->pageCount();
    const int start = (lastSearchPage_ + 1) % qMax(1, count);
    for (int offset = 0; offset < count; ++offset) {
        const int page = (start + offset) % count;
        if (document_->getAllText(page).text().contains(query, Qt::CaseInsensitive)) {
            lastSearchPage_ = page;
            pageSpin_->setValue(page + 1);
            view_->pageNavigator()->jump(page);
            statusLabel_->setText(QString("Texte trouvé à la page %1.").arg(page + 1));
            return;
        }
    }
    statusLabel_->setText("Aucun résultat trouvé dans le document.");
}

QString PdfPage::notesPath() const
{
    return currentPath_ + ".nexus-notes.json";
}

void PdfPage::editPageNote()
{
    if (currentPath_.isEmpty()) return;
    QJsonArray entries;
    QFile existing(notesPath());
    if (existing.open(QIODevice::ReadOnly)) {
        const auto parsed = QJsonDocument::fromJson(existing.readAll());
        if (parsed.isArray()) entries = parsed.array();
    }
    const int page = pageSpin_->value();
    QString previous;
    for (const auto &value : entries) {
        const auto object = value.toObject();
        if (object.value("page").toInt() == page) previous = object.value("comment").toString();
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QString("Commentaire — page %1").arg(page));
    auto *form = new QVBoxLayout(&dialog);
    form->addWidget(new QLabel("Votre commentaire pour cette page :", &dialog));
    auto *editor = new QTextEdit(&dialog);
    editor->setPlainText(previous);
    editor->setMinimumSize(380, 150);
    form->addWidget(editor);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    form->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;

    QJsonArray updated;
    for (const auto &value : entries)
        if (value.toObject().value("page").toInt() != page) updated.append(value);
    const QString comment = editor->toPlainText().trimmed();
    if (!comment.isEmpty()) {
        QJsonObject note;
        note.insert("page", page);
        note.insert("comment", comment);
        updated.append(note);
    }
    QFile output(notesPath());
    if (!output.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, "Nexus PDF", "Impossible d'enregistrer le commentaire.");
        return;
    }
    output.write(QJsonDocument(updated).toJson(QJsonDocument::Indented));
    statusLabel_->setText(comment.isEmpty() ? "Commentaire supprimé." : "Commentaire enregistré.");
}

void PdfPage::printPdf()
{
    if (document_->status() != QPdfDocument::Status::Ready) return;
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle("Imprimer le PDF");
    if (dialog.exec() != QDialog::Accepted) return;

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::warning(this, "Nexus PDF", "Impossible de démarrer l'impression.");
        return;
    }
    for (int page = 0; page < document_->pageCount(); ++page) {
        if (page > 0) printer.newPage();
        const QSize pixelSize = printer.pageRect(QPrinter::DevicePixel).size().toSize();
        const QImage image = document_->render(page, pixelSize);
        if (image.isNull()) continue;
        const QRect target = printer.pageRect(QPrinter::DevicePixel);
        painter.drawImage(target, image);
    }
    painter.end();
    statusLabel_->setText("Document envoyé à l'impression.");
}
