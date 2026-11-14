#pragma once
#include <QWidget>
class QTableWidget;
class QLabel;
class SheetPage final : public QWidget {
    Q_OBJECT
public:
    explicit SheetPage(QWidget *parent = nullptr);
private:
    QTableWidget *table_ = nullptr;
    QLabel *status_ = nullptr;
    void newSheet();
    void openCsv();
    void saveCsv();
    void openXlsx();
    void saveXlsx();
    void sortByCurrentColumn();
    void filterRows();
    void clearFilter();
    void addRow();
    void addColumn();
    void recalculate();
    QString cellValue(int row, int column, int depth = 0) const;
};
