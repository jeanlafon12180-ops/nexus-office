#include "sheetpage.h"

#include <QLabel>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTest>
#include <QStandardPaths>
#include <QPushButton>

class SheetsUiTest final : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
    }

    void initialGridAndTabsExist()
    {
        SheetPage page;
        auto *tabs = page.findChild<QTabWidget *>();
        auto *table = page.findChild<QTableWidget *>("sheetsTable");
        QVERIFY(tabs);
        QVERIFY(table);
        QCOMPARE(tabs->count(), 1);
        QCOMPARE(table->rowCount(), 30);
        QCOMPARE(table->columnCount(), 12);
    }

    void formulasRecalculateWhenCellChanges()
    {
        SheetPage page;
        auto *table = page.findChild<QTableWidget *>("sheetsTable");
        QVERIFY(table);

        table->setItem(0, 0, new QTableWidgetItem("12"));
        table->setItem(0, 1, new QTableWidgetItem("=A1*2"));
        QTRY_COMPARE(table->item(0, 1)->text(), QString("24"));
    }

    void addingSheetCreatesSecondTab()
    {
        SheetPage page;
        auto *tabs = page.findChild<QTabWidget *>();
        QVERIFY(tabs);

        const auto buttons = page.findChildren<QPushButton *>();
        QPushButton *newButton = nullptr;
        for (QPushButton *button : buttons) {
            if (button->text() == "Nouveau") {
                newButton = button;
                break;
            }
        }
        QVERIFY(newButton);
        QTest::mouseClick(newButton, Qt::LeftButton);
        QCOMPARE(tabs->count(), 2);
        QCOMPARE(tabs->tabText(1), QString("Feuille 2"));
    }
};

QTEST_MAIN(SheetsUiTest)
#include "sheets_ui_test.moc"
