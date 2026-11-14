#include "slidespage.h"
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPageLayout>
#include <QPageSize>
#include <QPrinter>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextImageFormat>
#include <QVBoxLayout>
#include <QHBoxLayout>

SlidesPage::SlidesPage(QWidget *parent) : QWidget(parent)
{
    auto *root=new QVBoxLayout(this);
    root->setContentsMargins(28,24,28,24);
    auto *bar=new QHBoxLayout;
    auto *heading=new QLabel("Nexus Slides",this);
    heading->setStyleSheet("font-size:27px;font-weight:750;color:#172033;background:transparent");
    bar->addWidget(heading); bar->addStretch();
    auto addButton=[this,bar](const QString &label,auto callback){
        auto *button=new QPushButton(label,this); bar->addWidget(button);
        connect(button,&QPushButton::clicked,this,callback);
    };
    addButton("Nouvelle diapositive",[this]{addSlide();});
    addButton("Supprimer",[this]{removeSlide();});
    addButton("Insérer une image",[this]{insertImage();});
    addButton("Exporter PDF",[this]{exportPdf();});
    root->addLayout(bar);
    auto *options=new QHBoxLayout;
    options->addWidget(new QLabel("Thème :",this));
    theme_=new QComboBox(this); theme_->addItems({"Clair","Bleu","Sombre"});
    options->addWidget(theme_); options->addStretch(); root->addLayout(options);
    connect(theme_,&QComboBox::currentTextChanged,this,[this]{applyTheme();});
    auto *hint=new QLabel("Créez plusieurs diapositives, ajoutez des images et des notes, puis exportez la présentation en PDF.",this);
    hint->setWordWrap(true); root->addWidget(hint);
    tabs_=new QTabWidget(this); root->addWidget(tabs_,1);
    status_=new QLabel("Prêt",this); root->addWidget(status_);
    addSlide(); tabs_->setTabText(0,"Diapositive 1"); nextSlideNumber_=2;
}
SlidesPage::Slide *SlidesPage::currentSlide()
{
    const int index=tabs_->currentIndex();
    return index>=0 && index<slides_.size() ? &slides_[index] : nullptr;
}
void SlidesPage::addSlide()
{
    Slide slide; slide.page=new QWidget(tabs_);
    auto *layout=new QVBoxLayout(slide.page); layout->setContentsMargins(18,18,18,18);
    slide.title=new QLineEdit(slide.page); slide.title->setPlaceholderText("Titre de la diapositive");
    layout->addWidget(slide.title);
    slide.body=new QTextEdit(slide.page); slide.body->setAcceptRichText(true);
    slide.body->setPlaceholderText("Contenu de la diapositive…"); layout->addWidget(slide.body,1);
    slide.notes=new QTextEdit(slide.page); slide.notes->setPlaceholderText("Notes du présentateur (non imprimées)…");
    slide.notes->setMaximumHeight(72); layout->addWidget(slide.notes);
    slides_.push_back(slide);
    tabs_->addTab(slide.page,"Diapositive "+QString::number(nextSlideNumber_++));
    tabs_->setCurrentWidget(slide.page); applyTheme();
    status_->setText(QString::number(slides_.size())+" diapositive(s)");
}
void SlidesPage::removeSlide()
{
    if(slides_.size()<=1){QMessageBox::information(this,"Nexus Slides","Une présentation doit conserver au moins une diapositive.");return;}
    const int index=tabs_->currentIndex(); if(index<0 || index>=slides_.size())return;
    QWidget *page=tabs_->widget(index); tabs_->removeTab(index); slides_.removeAt(index); page->deleteLater();
    status_->setText(QString::number(slides_.size())+" diapositive(s)");
}
void SlidesPage::insertImage()
{
    Slide *slide=currentSlide(); if(!slide)return;
    const QString path=QFileDialog::getOpenFileName(this,"Insérer une image",QString(),"Images (*.png *.jpg *.jpeg *.bmp *.gif *.webp)");
    if(path.isEmpty())return;
    QTextImageFormat image; image.setName(path); image.setWidth(420);
    QTextCursor cursor=slide->body->textCursor(); cursor.insertBlock(); cursor.insertImage(image);
    slide->body->setTextCursor(cursor); status_->setText("Image insérée : "+QFileInfo(path).fileName());
}
void SlidesPage::applyTheme()
{
    const QString selected=theme_->currentText();
    QString bg="#ffffff",fg="#172033",accent="#2346a0";
    if(selected=="Bleu"){bg="#eaf1ff";fg="#14294d";accent="#2457b8";}
    else if(selected=="Sombre"){bg="#202938";fg="#f4f7fb";accent="#8eb5ff";}
    for(auto &slide:slides_){
        slide.body->setStyleSheet(QString("padding:14px;font-size:17px;background:%1;color:%2;border:1px solid #aab5c5;border-radius:8px").arg(bg,fg));
        slide.title->setStyleSheet(QString("padding:10px;font-size:22px;font-weight:700;background:%1;color:%2;border:2px solid %3;border-radius:8px").arg(bg,fg,accent));
    }
    status_->setText(QString::number(slides_.size())+" diapositive(s) · thème "+selected.toLower());
}
void SlidesPage::exportPdf()
{
    QString path=QFileDialog::getSaveFileName(this,"Exporter la présentation en PDF","Presentation.pdf","Fichier PDF (*.pdf)");
    if(path.isEmpty())return; if(QFileInfo(path).suffix().isEmpty())path+=".pdf";
    QString bg="#ffffff",fg="#172033",accent="#2346a0";
    const QString selected=theme_->currentText();
    if(selected=="Bleu"){bg="#eaf1ff";fg="#14294d";accent="#2457b8";}
    else if(selected=="Sombre"){bg="#202938";fg="#f4f7fb";accent="#8eb5ff";}
    QString html="<html><head><meta charset='utf-8'><style>body{font-family:Arial;margin:0}.slide{page-break-after:always;min-height:650px;padding:42px}.slide:last-child{page-break-after:auto}h1{font-size:30pt}.content{font-size:18pt}</style></head><body>";
    for(const auto &slide:slides_){
        html+=QString("<section class='slide' style='background:%1;color:%2'><h1 style='color:%3'>%4</h1><div class='content'>%5</div></section>")
            .arg(bg,fg,accent,slide.title->text().toHtmlEscaped(),slide.body->toHtml());
    }
    html+="</body></html>";
    QTextDocument document; document.setHtml(html);
    QPrinter printer(QPrinter::HighResolution); printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path); printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Landscape); document.print(&printer);
    status_->setText("Présentation exportée en PDF : "+QFileInfo(path).fileName());
}
