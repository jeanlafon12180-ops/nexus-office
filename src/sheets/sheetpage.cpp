#include "sheetpage.h"
#include "formulaengine.h"
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QTableWidget>\n#include <QTabWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSet>
#include <QProcess>
#include <QTemporaryDir>
#include <QDir>
#include <QPainter>
#include <QDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QStandardPaths>
#include <QUrl>
#include <QTableWidgetSelectionRange>
#include <functional>

namespace {
constexpr int FormulaSourceRole = Qt::UserRole;
constexpr int FormulaRenderedRole = Qt::UserRole + 1;
}


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
    add("Ouvrir XLSX", [this]{openXlsx();});
    add("Enregistrer XLSX", [this]{saveXlsx();});
    add("Trier la colonne", [this]{sortByCurrentColumn();});
    add("Filtrer", [this]{filterRows();});
    add("Effacer filtre", [this]{clearFilter();});
    root->addLayout(top);
    auto *hint = new QLabel("Tableur CSV : modifiez les cellules. Formules : +, -, *, /, parenthèses, références de cellules et fonctions SUM, AVERAGE, MIN, MAX, COUNT sur une plage.",this);
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
void SheetPage::newSheet()
{
    auto *sheet = new QTableWidget(30,12,tabs_);
    sheet->setObjectName("sheetsTable");
    sheet->setAlternatingRowColors(true);
    sheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    sheet->verticalHeader()->setDefaultSectionSize(28);
    connect(sheet,&QTableWidget::cellChanged,this,[this,sheet] {
        if (table_ == sheet) recalculate();
    });
    const int index = tabs_->addTab(sheet, "Feuille " + QString::number(nextSheetNumber_++));
    tabs_->setCurrentIndex(index);
    table_ = sheet;
    status_->setText("Nouvelle feuille ajoutée au classeur.");
}
void SheetPage::addRow(){table_->insertRow(table_->rowCount());}
void SheetPage::addColumn(){table_->insertColumn(table_->columnCount());}
QString SheetPage::cellValue(int row,int col,int depth) const {
    if (depth > 20) return "#CYCLE!";
    if (row < 0 || col < 0 || row >= table_->rowCount() || col >= table_->columnCount())
        return "#REF!";
    const auto *item = table_->item(row, col);
    if (!item) return "0";
    const QVariant storedSource = item->data(FormulaSourceRole);
    const QString raw = (storedSource.isValid() ? storedSource.toString() : item->data(Qt::EditRole).toString()).trimmed();
    if (!raw.startsWith('=')) return raw.isEmpty() ? "0" : raw;
    return nexus::sheets::FormulaEngine::evaluate(raw, [this, depth](int refRow, int refColumn) {
        return cellValue(refRow, refColumn, depth + 1);
    });
}
void SheetPage::recalculate(){
    table_->blockSignals(true);
    for(int r=0;r<table_->rowCount();++r)for(int c=0;c<table_->columnCount();++c){
        auto *it=table_->item(r,c); if(!it)continue;
        const QString visible = it->data(Qt::DisplayRole).toString();
        const QVariant storedSource = it->data(FormulaSourceRole);
        const QVariant lastRendered = it->data(FormulaRenderedRole);
        QString source = storedSource.isValid() ? storedSource.toString() : visible;
        // DisplayRole and EditRole share the same Qt item role. Keep the
        // original formula separately so showing a result never destroys it.
        if (!storedSource.isValid() || (lastRendered.isValid() && visible != lastRendered.toString())) {
            source = visible;
            it->setData(FormulaSourceRole, source);
        }
        if(source.trimmed().startsWith('=')) {
            const QString result = cellValue(r,c);
            it->setData(Qt::DisplayRole, result);
            it->setData(FormulaRenderedRole, result);
            it->setToolTip("Formule : " + source + "\nRésultat : " + result);
        } else {
            it->setData(Qt::DisplayRole, source);
            it->setData(FormulaRenderedRole, source);
            it->setData(FormulaSourceRole, source);
            it->setToolTip(QString());
        }
    }
    table_->blockSignals(false);
    status_->setText("Tableau recalculé · résultats des formules affichés dans les cellules");
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

void SheetPage::openXlsx()
{
    const QString path = QFileDialog::getOpenFileName(this, "Ouvrir un classeur Excel", QString(), "Classeur Excel (*.xlsx)");
    if (path.isEmpty()) return;
    const QString soffice = QStandardPaths::findExecutable("soffice");
    if (soffice.isEmpty()) { QMessageBox::warning(this, "Import XLSX", "LibreOffice Calc est nécessaire pour importer les fichiers XLSX."); return; }
    QTemporaryDir temp;
    if (!temp.isValid()) return;
    QProcess process;
    process.start(soffice, {"-env:UserInstallation=" + QUrl::fromLocalFile(temp.filePath("profile")).toString(),
        "--headless", "--convert-to", "csv:Text - txt - csv (StarCalc)", "--outdir", temp.path(), path});
    if (!process.waitForStarted(5000) || !process.waitForFinished(60000) || process.exitCode() != 0) {
        QMessageBox::warning(this, "Import XLSX", QString::fromLocal8Bit(process.readAllStandardError())); return;
    }
    const QString csvPath = temp.filePath(QFileInfo(path).completeBaseName() + ".csv");
    QFile csv(csvPath);
    if (!csv.open(QIODevice::ReadOnly | QIODevice::Text)) { QMessageBox::warning(this, "Import XLSX", "Conversion CSV introuvable."); return; }
    table_->blockSignals(true); table_->clearContents(); table_->setRowCount(0); table_->setColumnCount(0);
    QTextStream in(&csv); int row = 0;
    while (!in.atEnd()) {
        const QString line = in.readLine();
        QStringList values; QString value; bool quoted = false;
        for (qsizetype i=0;i<line.size();++i) {
            const QChar ch=line[i];
            if (ch=='"') { if (quoted && i+1<line.size() && line[i+1]=='"') { value+='"'; ++i; } else quoted=!quoted; }
            else if (ch==';' && !quoted) { values<<value; value.clear(); }
            else value+=ch;
        }
        values<<value;
        if (table_->columnCount()<values.size()) table_->setColumnCount(values.size());
        table_->insertRow(row);
        for (int col=0;col<values.size();++col) table_->setItem(row,col,new QTableWidgetItem(values[col]));
        ++row;
    }
    if (table_->columnCount()==0) table_->setColumnCount(1);
    table_->setRowCount(qMax(30,table_->rowCount())); table_->blockSignals(false); recalculate();
    status_->setText("Classeur XLSX importé (première feuille) : "+QFileInfo(path).fileName());
}

void SheetPage::saveXlsx()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter en classeur Excel", "Classeur.xlsx", "Classeur Excel (*.xlsx)");
    if (path.isEmpty()) return;
    const QString soffice = QStandardPaths::findExecutable("soffice");
    if (soffice.isEmpty()) { QMessageBox::warning(this, "Export XLSX", "LibreOffice Calc est nécessaire pour exporter en XLSX."); return; }
    QTemporaryDir temp;
    if (!temp.isValid()) return;
    const QString csvPath = temp.filePath("NexusSheets.csv");
    QFile csv(csvPath);
    if (!csv.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&csv);
    for (int row=0;row<table_->rowCount();++row) {
        QStringList values;
        for (int col=0;col<table_->columnCount();++col) {
            QString value = table_->item(row,col) ? table_->item(row,col)->data(FormulaSourceRole).toString() : QString();
            value.replace('"',"\"\"");
            if (value.contains(';') || value.contains('"') || value.contains('\n')) value='"'+value+'"';
            values<<value;
        }
        out<<values.join(';')<<'\n';
    }
    csv.close();
    QProcess process;
    process.start(soffice, {"-env:UserInstallation=" + QUrl::fromLocalFile(temp.filePath("profile")).toString(),
        "--headless", "--convert-to", "xlsx", "--outdir", temp.path(), csvPath});
    if (!process.waitForStarted(5000) || !process.waitForFinished(60000) || process.exitCode()!=0) {
        QMessageBox::warning(this,"Export XLSX",QString::fromLocal8Bit(process.readAllStandardError())); return;
    }
    const QString generated=temp.filePath("NexusSheets.xlsx");
    if (!QFileInfo::exists(generated) || !QFile::copy(generated,path)) {
        QMessageBox::warning(this,"Export XLSX","Le fichier XLSX n'a pas pu être créé à cet emplacement."); return;
    }
    status_->setText("Export XLSX terminé (feuille active) : "+QFileInfo(path).fileName());
}

void SheetPage::sortByCurrentColumn()
{
    const int column=table_->currentColumn();
    if (column<0) { QMessageBox::information(this,"Tri","Sélectionnez une colonne à trier."); return; }
    table_->setSortingEnabled(true);
    table_->sortItems(column, Qt::AscendingOrder);
    status_->setText("Tri croissant appliqué à la colonne sélectionnée.");
}

void SheetPage::filterRows()
{
    const int column=table_->currentColumn();
    if (column<0) { QMessageBox::information(this,"Filtre","Sélectionnez une cellule dans la colonne à filtrer."); return; }
    bool accepted=false;
    const QString query=QInputDialog::getText(this,"Filtrer les lignes","Afficher les lignes contenant :",QLineEdit::Normal,QString(),&accepted);
    if (!accepted) return;
    for (int row=0;row<table_->rowCount();++row) {
        const auto *item=table_->item(row,column);
        const QString value=item?item->data(Qt::DisplayRole).toString():QString();
        table_->setRowHidden(row,!value.contains(query,Qt::CaseInsensitive));
    }
    status_->setText("Filtre appliqué à la colonne sélectionnée.");
}

void SheetPage::clearFilter()
{
    for (int row=0;row<table_->rowCount();++row) table_->setRowHidden(row,false);
    status_->setText("Filtre effacé.");
}
void SheetPage::saveCsv(){
    const QString path=QFileDialog::getSaveFileName(this,"Enregistrer le CSV","Tableau.csv","Fichiers CSV (*.csv)");if(path.isEmpty())return;
    QFile f(path);if(!f.open(QIODevice::WriteOnly|QIODevice::Text)){QMessageBox::warning(this,"Enregistrement impossible",f.errorString());return;}
    QTextStream out(&f);for(int r=0;r<table_->rowCount();++r){QStringList row;for(int c=0;c<table_->columnCount();++c){QString v=table_->item(r,c)?table_->item(r,c)->data(FormulaSourceRole).toString():QString();v.replace('"',"\"\"");if(v.contains(',')||v.contains('"')||v.contains('\n'))v='"'+v+'"';row<<v;}out<<row.join(',')<<'\n';}status_->setText("Enregistré : "+QFileInfo(path).fileName());
}
