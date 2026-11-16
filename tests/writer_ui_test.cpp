#include "writerpage.h"

#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QToolBar>
#include <QTest>
#include <QFont>

class WriterUiTest final : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
    }

    void editorAndMainActionsExist()
    {
        WriterPage page;
        auto *editor = page.findChild<QTextEdit *>("writerEditor");
        QVERIFY(editor);
        QVERIFY(page.findChild<QPushButton *>("writerPrimary"));
        QVERIFY(page.findChild<QToolBar *>("writerToolbar"));
        QVERIFY(page.findChild<QPushButton *>(QString(), Qt::FindDirectChildrenOnly) == nullptr
                || page.findChildren<QPushButton *>().size() >= 7);
    }

    void typingUpdatesWordAndCharacterCount()
    {
        WriterPage page;
        auto *editor = page.findChild<QTextEdit *>("writerEditor");
        QVERIFY(editor);

        editor->setPlainText("Bonjour Nexus Office");
        const auto labels = page.findChildren<QLabel *>();
        QString count;
        for (QLabel *label : labels) {
            if (label->text().contains("mot") && label->text().contains("caractères")) {
                count = label->text();
                break;
            }
        }
        QCOMPARE(count, QString("3 mots · 20 caractères"));
        QVERIFY(editor->document()->isModified());
    }

    void undoAndRedoRestoreText()
    {
        WriterPage page;
        auto *editor = page.findChild<QTextEdit *>("writerEditor");
        QVERIFY(editor);

        editor->setPlainText("Texte initial");
        editor->moveCursor(QTextCursor::End);
        editor->insertPlainText(" ajouté");
        QCOMPARE(editor->toPlainText(), QString("Texte initial ajouté"));

        editor->undo();
        QCOMPARE(editor->toPlainText(), QString("Texte initial"));
        editor->redo();
        QCOMPARE(editor->toPlainText(), QString("Texte initial ajouté"));
    }

    void formattingActionsAreAvailable()
    {
        WriterPage page;
        auto *toolbar = page.findChild<QToolBar *>("writerToolbar");
        QVERIFY(toolbar);
        QVERIFY(toolbar->actions().size() >= 15);

        QAction *bold = nullptr;
        for (QAction *action : toolbar->actions()) {
            if (action->text() == "Gras") {
                bold = action;
                break;
            }
        }
        QVERIFY(bold);
        QVERIFY(bold->isCheckable());
    }
};

QTEST_MAIN(WriterUiTest)
#include "writer_ui_test.moc"
