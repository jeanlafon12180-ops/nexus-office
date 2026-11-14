#pragma once

#include <QString>
#include <QRegularExpression>
#include <QSet>
#include <functional>
#include <cmath>
#include <utility>

namespace nexus::sheets {

class FormulaEngine {
public:
    using CellResolver = std::function<QString(int, int)>;

    static QString evaluate(const QString &formula, const CellResolver &resolve)
    {
        if (!formula.trimmed().startsWith('='))
            return formula;
        FormulaEngine parser(formula.trimmed().mid(1), resolve);
        const double value = parser.expression();
        parser.skipSpace();
        if (!parser.error_.isEmpty()) return parser.error_;
        if (parser.position_ != parser.source_.size()) return "#FORMULE!";
        return QString::number(value, 'g', 12);
    }

private:
    FormulaEngine(QString source, CellResolver resolve)
        : source_(std::move(source)), resolve_(std::move(resolve)) {}

    QString source_;
    CellResolver resolve_;
    qsizetype position_ = 0;
    QString error_;

    void skipSpace() { while (position_ < source_.size() && source_[position_].isSpace()) ++position_; }
    bool take(QChar ch) { skipSpace(); if (position_ < source_.size() && source_[position_] == ch) { ++position_; return true; } return false; }
    void fail(const QString &message) { if (error_.isEmpty()) error_ = message; }

    double expression()
    {
        double value = term();
        while (error_.isEmpty()) {
            if (take('+')) value += term();
            else if (take('-')) value -= term();
            else break;
        }
        return value;
    }

    double term()
    {
        double value = factor();
        while (error_.isEmpty()) {
            if (take('*')) value *= factor();
            else if (take('/')) {
                const double divisor = factor();
                if (divisor == 0.0) { fail("#DIV/0!"); return 0; }
                value /= divisor;
            } else break;
        }
        return value;
    }

    static bool columnNumber(const QString &letters, int *column)
    {
        int result = 0;
        for (QChar ch : letters.toUpper()) {
            if (ch < 'A' || ch > 'Z') return false;
            result = result * 26 + ch.unicode() - 'A' + 1;
        }
        *column = result - 1;
        return true;
    }

    double cell(int row, int column)
    {
        if (row < 0 || column < 0 || row >= 100000 || column >= 16384) {
            fail("#REF!");
            return 0;
        }
        bool ok = false;
        const QString raw = resolve_(row, column).trimmed();
        QString computed = raw;
        if (raw.startsWith('=')) {
            computed = evaluate(raw, resolve_);
            if (computed.startsWith('#')) { fail(computed); return 0; }
        }
        const double value = computed.toDouble(&ok);
        if (!ok) { fail("#VALEUR!"); return 0; }
        return value;
    }

    double factor()
    {
        skipSpace();
        if (take('+')) return factor();
        if (take('-')) return -factor();
        if (take('(')) {
            const double value = expression();
            if (!take(')')) fail("#FORMULE!");
            return value;
        }

        const qsizetype start = position_;
        while (position_ < source_.size() && source_[position_].isLetter()) ++position_;
        const QString letters = source_.mid(start, position_ - start);
        if (!letters.isEmpty()) {
            skipSpace();
            if (take('(')) return function(letters);
            const qsizetype digitsStart = position_;
            while (position_ < source_.size() && source_[position_].isDigit()) ++position_;
            bool ok = false;
            const int row = source_.mid(digitsStart, position_ - digitsStart).toInt(&ok) - 1;
            int column = -1;
            if (!ok || !columnNumber(letters, &column)) { fail("#FORMULE!"); return 0; }
            return cell(row, column);
        }

        const qsizetype numberStart = position_;
        bool decimal = false;
        while (position_ < source_.size()) {
            const QChar ch = source_[position_];
            if (ch.isDigit()) ++position_;
            else if (ch == '.' && !decimal) { decimal = true; ++position_; }
            else break;
        }
        bool ok = false;
        const double value = source_.mid(numberStart, position_ - numberStart).toDouble(&ok);
        if (!ok) fail("#FORMULE!");
        return value;
    }

    double function(const QString &name)
    {
        const qsizetype start = position_;
        while (position_ < source_.size() && source_[position_] != ')') ++position_;
        if (position_ >= source_.size()) { fail("#FORMULE!"); return 0; }
        const QString range = source_.mid(start, position_ - start).trimmed();
        ++position_;

        static const QRegularExpression rangePattern(
            "^([A-Z]+)([0-9]+):([A-Z]+)([0-9]+)$",
            QRegularExpression::CaseInsensitiveOption);
        const auto match = rangePattern.match(range);
        int c1 = 0, c2 = 0;
        if (!match.hasMatch() ||
            !columnNumber(match.captured(1), &c1) ||
            !columnNumber(match.captured(3), &c2)) {
            fail("#FORMULE!");
            return 0;
        }
        const int r1 = match.captured(2).toInt() - 1;
        const int r2 = match.captured(4).toInt() - 1;
        if (r1 < 0 || r2 < 0 || qAbs(r2-r1) > 9999 || qAbs(c2-c1) > 255) {
            fail("#REF!");
            return 0;
        }

        double total = 0, minimum = 0, maximum = 0;
        int count = 0;
        for (int row = qMin(r1,r2); row <= qMax(r1,r2); ++row) {
            for (int col = qMin(c1,c2); col <= qMax(c1,c2); ++col) {
                const QString raw = resolve_(row,col).trimmed();
                if (raw.isEmpty()) continue;
                bool ok = false;
                const QString computed = raw.startsWith('=') ? evaluate(raw, resolve_) : raw;
                const double value = computed.toDouble(&ok);
                if (!ok) { fail(computed.startsWith('#') ? computed : "#VALEUR!"); return 0; }
                total += value;
                if (count == 0) minimum = maximum = value;
                else { minimum = qMin(minimum,value); maximum = qMax(maximum,value); }
                ++count;
            }
        }
        const QString fn = name.toUpper();
        if (fn == "SUM") return total;
        if (fn == "AVERAGE" || fn == "MOYENNE") return count ? total / count : 0;
        if (fn == "MIN") return count ? minimum : 0;
        if (fn == "MAX") return count ? maximum : 0;
        if (fn == "COUNT" || fn == "NB") return count;
        fail("#NOM?");
        return 0;
    }
};

} // namespace nexus::sheets
