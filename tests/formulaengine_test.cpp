#include "../src/sheets/formulaengine.h"

#include <QHash>
#include <cassert>
#include <cmath>

int main()
{
    QHash<QString, QString> cells{
        {"A1", "10"}, {"A2", "20"}, {"A3", "30"},
        {"B1", "=A1+A2*2"}, {"B2", "=SUM(A1:A3)"},
        {"B3", "=AVERAGE(A1:A3)"}, {"B4", "=MIN(A1:A3)"},
        {"B5", "=MAX(A1:A3)"}, {"B6", "=COUNT(A1:A3)"},
        {"C1", "=(A1+A2)/3"}, {"C2", "=10/0"},
        {"C3", "=UNKNOWN(A1:A2)"}
    };

    auto resolve = [&cells](int row, int col) -> QString {
        QString letters;
        int n = col + 1;
        while (n > 0) {
            const int remainder = (n - 1) % 26;
            letters.prepend(QChar('A' + remainder));
            n = (n - 1) / 26;
        }
        return cells.value(letters + QString::number(row + 1), "0");
    };

    auto value = [&resolve](const QString &formula) {
        return nexus::sheets::FormulaEngine::evaluate(formula, resolve);
    };

    assert(value("=A1+A2*2") == "50");
    assert(value("=SUM(A1:A3)") == "60");
    assert(value("=AVERAGE(A1:A3)") == "20");
    assert(value("=MIN(A1:A3)") == "10");
    assert(value("=MAX(A1:A3)") == "30");
    assert(value("=COUNT(A1:A3)") == "3");
    assert(value("=(A1+A2)/3") == "10");
    assert(value("=10/0") == "#DIV/0!");
    assert(value("=UNKNOWN(A1:A2)") == "#NOM?");
    assert(value("=A99") == "0");
    return 0;
}
