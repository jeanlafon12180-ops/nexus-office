#include "slidespage.h"
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTextDocument>
#include <QPrinter>
#include <QPageSize>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
SlidesPage::SlidesPage(QWidget *parent):QWidget(parent){
    auto *root=new QVBoxLayout(this);root->setContentsMargins(28,24,28,24);
    auto *bar=new QHBoxLayout;auto *heading=new QLabel("Nexus Slides",this);heading->setStyleSheet("font-size:27px;font-weight:750;color:#172033;background:transparent");bar->addWidget(heading);bar->addStretch();
    auto *exportButton=new QPushButton("Exporter en PDF",this);bar->addWidget(exportButton);root->addLayout(bar);
    auto *hint=new QLabel("Éditeur de diapositives simple : renseignez le titre et le contenu, puis exportez une page PDF.",this);hint->setWordWrap(true);root->addWidget(hint);
    title_=new QLineEdit(this);title_->setPlaceholderText("Titre de la diapositive");title_->setStyleSheet("padding:12px;font-size:22px;background:white;color:#172033;border:1px solid #dce3ed;border-radius:8px");root->addWidget(title_);
    body_=new QTextEdit(this);body_->setPlaceholderText("Contenu de la diapositive…");body_->setStyleSheet("padding:18px;font-size:16px;background:white;color:#172033;border:1px solid #dce3ed;border-radius:8px");root->addWidget(body_,1);
    status_=new QLabel("1 diapositive · export PDF disponible",this);root->addWidget(status_);
    connect(exportButton,&QPushButton::clicked,this,&SlidesPage::exportPdf);
}
void SlidesPage::exportPdf(){
    QString path=QFileDialog::getSaveFileName(this,"Exporter la diapositive en PDF","Presentation.pdf","Fichier PDF (*.pdf)");if(path.isEmpty())return;if(QFileInfo(path).suffix().isEmpty())path+=".pdf";
    QTextDocument doc;doc.setHtml(QString("<html><body style='font-family:Arial;padding:48px'><h1 style='font-size:30pt;color:#2346a0'>%1</h1><div style='font-size:18pt;white-space:pre-wrap'>%2</div><p style='color:#778'>Créé avec Nexus Slides</p></body></html>").arg(title_->text().toHtmlEscaped(),body_->toPlainText().toHtmlEscaped().replace("\n","<br>")));
    QPrinter printer(QPrinter::HighResolution);printer.setOutputFormat(QPrinter::PdfFormat);printer.setOutputFileName(path);printer.setPageSize(QPageSize(QPageSize::A4));doc.print(&printer);
    status_->setText("PDF exporté : "+QFileInfo(path).fileName());
}
