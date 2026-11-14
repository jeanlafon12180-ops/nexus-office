#pragma once
#include <QWidget>
#include <QString>

class QTableWidget;
class QTabWidget;
class QLabel;

class SheetPage final : public QWidget {
    Q_OBJECT
public:
    explicit SheetPage(QWidget *parent = nullptr);
private:
    QTableWidget *table_ = nullptr;
    QTabWidget *tabs_ = nullptr;
    int nextSheetNumber_ = 2;
    QLabel *status_ = nullptr;
    void newSheet();
    void openCsv();
    void saveCsv();
    void openXlsx();
    void saveXlsx();
    void sortByCurrentColumn();
    void filterRows();
    void clearFilter();
    void showChart();
    void addRow();
    void addColumn();
    void recalculate();
    QString cellValue(int row, int column, int depth = 0) const;
};
