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
#include <QTextTable>
#include <QTextTableFormat>
#include <QInputDialog>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <QUrl>
#include <QGraphicsOpacityEffect>
#include <QStackedWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QProcess>
#include <QTemporaryDir>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
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
    addButton("Insérer un tableau",[this]{insertTable();});
    addButton("Insérer une forme",[this]{insertShape();});
    addButton("Diaporama",[this]{startSlideshow();});
    addButton("Ouvrir PPTX",[this]{importPptx();});
    addButton("Enregistrer PPTX",[this]{exportPptx();});
    addButton("Exporter PDF",[this]{exportPdf();});
    root->addLayout(bar);
    auto *options=new QHBoxLayout;
    options->addWidget(new QLabel("Thème :",this));
    theme_=new QComboBox(this); theme_->addItems({"Clair","Bleu","Sombre"});
    options->addWidget(theme_);
    options->addWidget(new QLabel("Transition :",this));
    transition_=new QComboBox(this); transition_->addItems({"Aucune","Fondu","Glissement"});
    options->addWidget(transition_); options->addStretch(); root->addLayout(options);
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
void SlidesPage::insertTable()
{
    Slide *slide=currentSlide();
    if(!slide)return;
    bool rowsOk=false,columnsOk=false;
    const int rows=QInputDialog::getInt(this,"Insérer un tableau","Nombre de lignes :",3,1,20,1,&rowsOk);
    if(!rowsOk)return;
    const int columns=QInputDialog::getInt(this,"Insérer un tableau","Nombre de colonnes :",3,1,12,1,&columnsOk);
    if(!columnsOk)return;
    QTextTableFormat format;
    format.setBorder(1);
    format.setCellPadding(6);
    format.setCellSpacing(0);
    format.setHeaderRowCount(1);
    QTextCursor cursor=slide->body->textCursor();
    cursor.insertBlock();
    QTextTable *table=cursor.insertTable(rows,columns,format);
    for(int row=0;row<rows;++row){
        for(int column=0;column<columns;++column){
            QTextTableCell cell=table->cellAt(row,column);
            QTextCursor cellCursor=cell.firstCursorPosition();
            cellCursor.insertText(row==0 ? "En-tête" : "Texte");
        }
    }
    slide->body->setTextCursor(table->lastCursorPosition());
    status_->setText("Tableau ajouté à la diapositive.");
}
void SlidesPage::insertShape()
{
    Slide *slide=currentSlide();
    if(!slide)return;
    const QStringList shapes={"Encadré","Bandeau","Citation"};
    bool accepted=false;
    const QString choice=QInputDialog::getItem(this,"Insérer une forme","Type de bloc :",shapes,0,false,&accepted);
    if(!accepted)return;
    const QString text=QInputDialog::getText(this,"Texte de la forme","Contenu :");
    QString style="display:block;padding:18px;margin:12px 0;border:2px solid #2457b8;border-radius:12px;";
    if(choice=="Bandeau")style+="background-color:#eaf1ff;font-weight:bold;text-align:center;";
    else if(choice=="Citation")style+="border-left:6px solid #2457b8;font-style:italic;";
    else style+="background-color:#f4f7fb;";
    QTextCursor cursor=slide->body->textCursor();
    cursor.insertBlock();
    cursor.insertHtml(QString("<div style='%1'>%2</div>").arg(style,text.toHtmlEscaped()));
    cursor.insertBlock();
    status_->setText(choice+" ajouté à la diapositive.");
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
void SlidesPage::startSlideshow()
{
    if(slides_.isEmpty())return;
    QDialog dialog(this);
    dialog.setWindowTitle("Nexus Slides — Diaporama");
    dialog.resize(1000,650);
    auto *layout=new QVBoxLayout(&dialog);
    auto *stack=new QStackedWidget(&dialog);
    for(const auto &slide:slides_){
        auto *page=new QWidget(stack);
        auto *pageLayout=new QVBoxLayout(page);
        auto *title=new QLabel(slide.title->text(),page);
        title->setStyleSheet("font-size:34px;font-weight:700;padding:24px;");
        auto *body=new QTextEdit(page);
        body->setReadOnly(true); body->setHtml(slide.body->toHtml());
        body->setStyleSheet("font-size:22px;border:0;padding:24px;");
        pageLayout->addWidget(title); pageLayout->addWidget(body,1);
        stack->addWidget(page);
    }
    layout->addWidget(stack,1);
    auto *controls=new QHBoxLayout;
    auto *previous=new QPushButton("Précédent",&dialog);
    auto *next=new QPushButton("Suivant",&dialog);
    auto *close=new QPushButton("Terminer",&dialog);
    auto *counter=new QLabel(&dialog);
    controls->addWidget(previous); controls->addWidget(counter); controls->addStretch();
    controls->addWidget(next); controls->addWidget(close); layout->addLayout(controls);
    auto update=[&]{
        const int index=stack->currentIndex();
        counter->setText(QString("Diapositive %1 / %2").arg(index+1).arg(stack->count()));
        previous->setEnabled(index>0); next->setEnabled(index+1<stack->count());
    };
    auto change=[&](int index){
        if(index<0 || index>=stack->count())return;
        const QString transition=transition_->currentText();
        if(transition=="Aucune"){stack->setCurrentIndex(index);update();return;}
        QWidget *current=stack->currentWidget();
        auto *effect=new QGraphicsOpacityEffect(current);
        current->setGraphicsEffect(effect);
        auto *animation=new QPropertyAnimation(effect,"opacity",&dialog);
        animation->setDuration(220);
        animation->setStartValue(transition=="Fondu"?1.0:0.55);
        animation->setEndValue(transition=="Fondu"?0.15:1.0);
        connect(animation,&QPropertyAnimation::finished,&dialog,[&,index,current,effect]{
            current->setGraphicsEffect(nullptr);
            stack->setCurrentIndex(index);
            update();
        });
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    };
    connect(previous,&QPushButton::clicked,&dialog,[&]{change(stack->currentIndex()-1);});
    connect(next,&QPushButton::clicked,&dialog,[&]{change(stack->currentIndex()+1);});
    connect(close,&QPushButton::clicked,&dialog,&QDialog::accept);
    update();
    dialog.exec();
}

void SlidesPage::importPptx()
{
    const QString path=QFileDialog::getOpenFileName(this,"Importer une présentation PowerPoint",QString(),"PowerPoint (*.pptx)");
    if(path.isEmpty())return;
    const QString soffice=QStandardPaths::findExecutable("soffice");
    if(soffice.isEmpty()){QMessageBox::warning(this,"Import PPTX","LibreOffice Impress est nécessaire pour importer un PPTX.");return;}
    QTemporaryDir temp;
    if(!temp.isValid())return;
    QProcess process;
    process.start(soffice,{"-env:UserInstallation="+QUrl::fromLocalFile(temp.filePath("profile")).toString(),
        "--headless","--convert-to","fodp","--outdir",temp.path(),path});
    if(!process.waitForStarted(5000)||!process.waitForFinished(60000)||process.exitCode()!=0){
        QMessageBox::warning(this,"Import PPTX",QString::fromLocal8Bit(process.readAllStandardError()));return;
    }
    QFile file(temp.filePath(QFileInfo(path).completeBaseName()+".fodp"));
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){QMessageBox::warning(this,"Import PPTX","Le fichier de conversion FODP est introuvable.");return;}
    QXmlStreamReader xml(&file);
    QVector<QPair<QString,QString>> imported;
    bool inPage=false; QString title,body; bool titleSeen=false;
    while(!xml.atEnd()){
        xml.readNext();
        if(xml.isStartElement()&&xml.name().toString()=="page"){
            inPage=true;title.clear();body.clear();titleSeen=false;
        } else if(inPage&&xml.isStartElement()&&xml.name().toString()=="p"){
            const QString text=xml.readElementText();
            if(!titleSeen){title=text;titleSeen=true;}else{if(!body.isEmpty())body+="\n";body+=text;}
        } else if(xml.isEndElement()&&xml.name()=="page"&&inPage){
            imported.push_back({title,body});inPage=false;
        }
    }
    if(xml.hasError()||imported.isEmpty()){QMessageBox::warning(this,"Import PPTX","Impossible de lire les diapositives du PPTX converti.");return;}
    for(auto &slide:slides_){tabs_->removeTab(tabs_->indexOf(slide.page));slide.page->deleteLater();}
    slides_.clear();
    for(const auto &entry:imported){
        addSlide();
        Slide *slide=currentSlide();
        slide->title->setText(entry.first);
        slide->body->setPlainText(entry.second);
    }
    status_->setText(QString("Importées : %1 diapositives (texte)").arg(imported.size()));
}

void SlidesPage::exportPptx()
{
    const QString path=QFileDialog::getSaveFileName(this,"Exporter PowerPoint", "Presentation.pptx","PowerPoint (*.pptx)");
    if(path.isEmpty())return;
    QString output=path;if(QFileInfo(output).suffix().isEmpty())output+=".pptx";
    const QString soffice=QStandardPaths::findExecutable("soffice");
    if(soffice.isEmpty()){QMessageBox::warning(this,"Export PPTX","LibreOffice Impress est nécessaire pour exporter un PPTX.");return;}
    QTemporaryDir temp;if(!temp.isValid())return;
    QString fodp="<?xml version=\\"1.0\\" encoding=\\"UTF-8\\"?>"
      "<office:document xmlns:office=\\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\\" "
      "xmlns:draw=\\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\\" "
      "xmlns:text=\\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\\" "
      "office:version=\\"1.2\\"><office:body><office:presentation>";
    for(int i=0;i<slides_.size();++i){
        const auto &slide=slides_[i];
        fodp+="<draw:page draw:name=\\"Slide"+QString::number(i+1)+"\\">";
        fodp+="<draw:frame><draw:text-box><text:p>"+slide.title->text().toHtmlEscaped()+"</text:p>";
        const QStringList lines=slide.body->toPlainText().split('\n');
        for(const QString &line:lines)fodp+="<text:p>"+line.toHtmlEscaped()+"</text:p>";
        fodp+="</draw:text-box></draw:frame></draw:page>";
    }
    fodp+="</office:presentation></office:body></office:document>";
    const QString input=temp.filePath("NexusSlides.fodp");
    QFile file(input);if(!file.open(QIODevice::WriteOnly|QIODevice::Text))return;
    file.write(fodp.toUtf8());file.close();
    QProcess process;
    process.start(soffice,{"-env:UserInstallation="+QUrl::fromLocalFile(temp.filePath("profile")).toString(),
        "--headless","--convert-to","pptx","--outdir",temp.path(),input});
    if(!process.waitForStarted(5000)||!process.waitForFinished(60000)||process.exitCode()!=0){
        QMessageBox::warning(this,"Export PPTX",QString::fromLocal8Bit(process.readAllStandardError()));return;
    }
    const QString generated=temp.filePath("NexusSlides.pptx");
    if(QFileInfo::exists(output))QFile::remove(output);
    if(!QFileInfo::exists(generated)||!QFile::copy(generated,output)){
        QMessageBox::warning(this,"Export PPTX","Le fichier PPTX n'a pas pu être créé.");return;
    }
    status_->setText("Export PowerPoint terminé (texte des diapositives).");
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
