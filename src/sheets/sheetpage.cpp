#include "sheetpage.h"
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QTableWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSet>
#include <functional>

SheetPage::SheetPage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28,24,28,24);
    auto *top = new QHBoxLayout;
    auto *title = new QLabel("Nexus Sheets", this);
    title->setStyleSheet("font-size:27px;font-weight:750;color:#172033;background:transparent");
    top->addWidget(title); top->addStretch();
    auto add = [&](const QString &text, auto slot) {
        auto *b = new QPushButton(text,this); top->addWidget(b);
        connect(b,&QPushButton::clicked,this,slot);
    };
    add("Nouveau", [this]{newSheet();});
    add("Ajouter une ligne", [this]{addRow();});
    add("Ajouter une colonne", [this]{addColumn();});
    add("Ouvrir CSV", [this]{openCsv();});
    add("Enregistrer CSV", [this]{saveCsv();});
    root->addLayout(top);
    auto *hint = new QLabel("Tableur CSV : modifiez les cellules. Formules prises en charge : =A1+B1, =A1*2 et =SUM(A1:B4).",this);
    hint->setWordWrap(true); hint->setStyleSheet("color:#52627a;background:transparent");
    root->addWidget(hint);
    table_ = new QTableWidget(30,12,this);
    table_->setObjectName("sheetsTable");
    table_->setAlternatingRowColors(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->verticalHeader()->setDefaultSectionSize(28);
    root->addWidget(table_,1);
    status_ = new QLabel("Prêt · 30 lignes · 12 colonnes",this);
    root->addWidget(status_);
    connect(table_,&QTableWidget::cellChanged,this,[this]{recalculate();});
    setStyleSheet("QTableWidget{background:white;color:#172033;gridline-color:#dce3ed;selection-background-color:#cbd8ff} QHeaderView::section{background:#eef2f8;color:#263449;padding:6px;border:0}");
}
void SheetPage::newSheet(){table_->blockSignals(true);table_->clearContents();table_->setRowCount(30);table_->setColumnCount(12);table_->blockSignals(false);status_->setText("Nouveau tableau");}
void SheetPage::addRow(){table_->insertRow(table_->rowCount());}
void SheetPage::addColumn(){table_->insertColumn(table_->columnCount());}
QString SheetPage::cellValue(int row,int col,int depth) const {
    if(depth>20 || row<0 || col<0 || row>=table_->rowCount() || col>=table_->columnCount()) return "0";
    auto *item=table_->item(row,col); if(!item) return "0";
    QString raw=item->data(Qt::EditRole).toString().trimmed(); if(!raw.startsWith('=')) return raw;
    raw=raw.mid(1).trimmed();
    auto colIndex=[](const QString &letters){int n=0;for(QChar ch:letters.toUpper()){if(!ch.isLetter())return -1;n=n*26+ch.unicode()-QChar('A').unicode()+1;}return n-1;};
    QRegularExpression sum("^SUM\\(([A-Z]+)([0-9]+):([A-Z]+)([0-9]+)\\)$",QRegularExpression::CaseInsensitiveOption);
    auto sm=sum.match(raw);
    if(sm.hasMatch()){
        int c1=colIndex(sm.captured(1)),r1=sm.captured(2).toInt()-1,c2=colIndex(sm.captured(3)),r2=sm.captured(4).toInt()-1;
        double total=0; for(int rr=qMin(r1,r2);rr<=qMax(r1,r2);++rr)for(int cc=qMin(c1,c2);cc<=qMax(c1,c2);++cc) total+=cellValue(rr,cc,depth+1).toDouble();
        return QString::number(total);
    }
    QRegularExpression ref("^([A-Z]+)([0-9]+)([+*/-])([A-Z]+)([0-9]+)$",QRegularExpression::CaseInsensitiveOption);
    auto rm=ref.match(raw);
    if(rm.hasMatch()){
        double a=cellValue(rm.captured(2).toInt()-1,colIndex(rm.captured(1)),depth+1).toDouble();
        double b=cellValue(rm.captured(5).toInt()-1,colIndex(rm.captured(4)),depth+1).toDouble();
        QString op=rm.captured(3); if(op=="+")return QString::number(a+b);if(op=="-")return QString::number(a-b);if(op=="*")return QString::number(a*b);return b==0?"#DIV/0!":QString::number(a/b);
    }
    QRegularExpression literal("^(-?[0-9]+(?:\\.[0-9]+)?)([+*/-])(-?[0-9]+(?:\\.[0-9]+)?)$");
    auto lm=literal.match(raw);
    if(lm.hasMatch()){double a=lm.captured(1).toDouble(),b=lm.captured(3).toDouble();QString op=lm.captured(2);if(op=="+")return QString::number(a+b);if(op=="-")return QString::number(a-b);if(op=="*")return QString::number(a*b);return b==0?"#DIV/0!":QString::number(a/b);}
    return "#FORMULE!";
}
void SheetPage::recalculate(){
    table_->blockSignals(true);
    for(int r=0;r<table_->rowCount();++r)for(int c=0;c<table_->columnCount();++c){
        auto *it=table_->item(r,c); if(!it)continue;
        const QString source = it->data(Qt::EditRole).toString();
        if(source.trimmed().startsWith('=')) {
            const QString result = cellValue(r,c);
            it->setData(Qt::DisplayRole, result);
            it->setToolTip("Formule : " + source + "\nRésultat : " + result);
        } else {
            it->setData(Qt::DisplayRole, source);
            it->setToolTip(QString());
        }
    }
    table_->blockSignals(false);
    status_->setText("Tableau modifié · les formules s’affichent dans l’infobulle des cellules");
}
void SheetPage::openCsv(){
    const QString path=QFileDialog::getOpenFileName(this,"Ouvrir un CSV",QString(),"Fichiers CSV (*.csv);;Tous les fichiers (*)");if(path.isEmpty())return;
    QFile f(path);if(!f.open(QIODevice::ReadOnly|QIODevice::Text)){QMessageBox::warning(this,"Ouverture impossible",f.errorString());return;}
    table_->blockSignals(true);table_->clearContents();table_->setRowCount(0);table_->setColumnCount(0);
    QTextStream in(&f);int row=0;while(!in.atEnd()){QString line=in.readLine();QStringList values;QString cur;bool quoted=false;
        for(int i=0;i<line.size();++i){QChar ch=line[i];if(ch=='"'){if(quoted&&i+1<line.size()&&line[i+1]=='"'){cur+='"';++i;}else quoted=!quoted;}else if(ch==','&&!quoted){values<<cur;cur.clear();}else cur+=ch;}values<<cur;
        if(table_->columnCount()<values.size())table_->setColumnCount(values.size());table_->insertRow(row);
        for(int c=0;c<values.size();++c)table_->setItem(row,c,new QTableWidgetItem(values[c]));++row;}
    if(table_->columnCount()==0)table_->setColumnCount(1);table_->setRowCount(qMax(30,table_->rowCount()));table_->blockSignals(false);recalculate();status_->setText("Ouvert : "+QFileInfo(path).fileName());
}
void SheetPage::saveCsv(){
    const QString path=QFileDialog::getSaveFileName(this,"Enregistrer le CSV","Tableau.csv","Fichiers CSV (*.csv)");if(path.isEmpty())return;
    QFile f(path);if(!f.open(QIODevice::WriteOnly|QIODevice::Text)){QMessageBox::warning(this,"Enregistrement impossible",f.errorString());return;}
    QTextStream out(&f);for(int r=0;r<table_->rowCount();++r){QStringList row;for(int c=0;c<table_->columnCount();++c){QString v=table_->item(r,c)?table_->item(r,c)->data(Qt::EditRole).toString():QString();v.replace('"',"\"\"");if(v.contains(',')||v.contains('"')||v.contains('\n'))v='"'+v+'"';row<<v;}out<<row.join(',')<<'\n';}status_->setText("Enregistré : "+QFileInfo(path).fileName());
}
