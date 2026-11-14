#include "pdfpage.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>
PdfPage::PdfPage(QWidget *parent):QWidget(parent){
    auto *layout=new QVBoxLayout(this);layout->setContentsMargins(28,24,28,24);
    auto *heading=new QLabel("Nexus PDF",this);heading->setStyleSheet("font-size:27px;font-weight:750;color:#172033;background:transparent");layout->addWidget(heading);
    auto *info=new QLabel("Ouvrez un PDF avec le lecteur PDF installé sur votre ordinateur. L’export PDF est disponible depuis Nexus Slides.",this);info->setWordWrap(true);layout->addWidget(info);
    auto *open=new QPushButton("Ouvrir un fichier PDF",this);open->setStyleSheet("background:#315be8;color:white;padding:12px 18px;border:0;border-radius:8px");layout->addWidget(open,0,Qt::AlignLeft);
    fileLabel_=new QLabel("Aucun fichier sélectionné",this);layout->addWidget(fileLabel_);layout->addStretch();
    connect(open,&QPushButton::clicked,this,&PdfPage::openPdf);
}
void PdfPage::openPdf(){QString path=QFileDialog::getOpenFileName(this,"Ouvrir un PDF",QString(),"Documents PDF (*.pdf)");if(path.isEmpty())return;if(!QDesktopServices::openUrl(QUrl::fromLocalFile(path))){fileLabel_->setText("Impossible d’ouvrir ce fichier avec l’application par défaut.");return;}fileLabel_->setText("Ouvert avec le lecteur PDF du système : "+QFileInfo(path).fileName());}
