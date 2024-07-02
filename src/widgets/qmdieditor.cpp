/**
 * \file qmdieditor
 * \brief Implementation of
 * \author Diego Iastrubni diegoiast@gmail.com
 * License GPL 2008
 * \see class name
 */

#include "qmdieditor.h"
#include "qmdihost.h"
#include "qmdiserver.h"
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QStyle>
#include <QTabWidget>
#include <QTextBlock>
#include <QTextDocument>
#include <textoperationswidget.h>

qmdiEditor::qmdiEditor(QString fName, QWidget *p) : Qutepart::Qutepart(p) {
    operationsWidget = new QsvTextOperationsWidget(this);

    QFont monospacedFont = this->font();
    monospacedFont.setPointSize(12);
    monospacedFont.setFamily("Monospace");
    setFont(monospacedFont);
    setLineWrapMode(LineWrapMode::NoWrap);

    setupActions();

    textOperationsMenu = new QMenu(tr("Text actions"), this);
    textOperationsMenu->setObjectName("qmdiEditor::textOperationsMenu");
    textOperationsMenu->addAction(actionCapitalize);
    textOperationsMenu->addAction(actionLowerCase);
    textOperationsMenu->addAction(actionChangeCase);

    bookmarksMenu = new QMenu(tr("Bookmarks"), this);
    bookmarksMenu->setObjectName("qmdiEditor::bookmarksMenu");
    bookmarksMenu->addAction(toggleBookmarkAction());
    bookmarksMenu->addSeparator();
    bookmarksMenu->addAction(nextBookmarkAction());
    bookmarksMenu->addAction(prevBookmarkAction());

    this->menus["&File"]->addAction(actionSave);
    this->menus["&Edit"]->addAction(actionUndo);
    this->menus["&Edit"]->addAction(actionRedo);
    this->menus["&Edit"]->addSeparator();
    this->menus["&Edit"]->addAction(actionCopy);
    this->menus["&Edit"]->addAction(actionCut);
    this->menus["&Edit"]->addAction(actionPaste);
    this->menus["&Edit"]->addSeparator();
    this->menus["&Edit"]->addMenu(textOperationsMenu);
    this->menus["&Edit"]->addMenu(bookmarksMenu);
    // menus["&Edit"]->addAction( actionTogglebreakpoint );
    // menus["&Edit"]->addAction(actionFindMatchingBracket);

    this->menus["&Search"]->addAction(actionFind);
    this->menus["&Search"]->addAction(actionFindNext);
    this->menus["&Search"]->addAction(actionFindPrev);
    // this->menus["&Search"]->addAction( actionClearSearchHighlight );
    this->menus["&Search"]->addSeparator();
    this->menus["&Search"]->addAction(actionReplace);
    this->menus["&Search"]->addSeparator();
    this->menus["&Search"]->addAction(actionGotoLine);

    this->toolbars[tr("main")]->addSeparator();
    this->toolbars[tr("main")]->addAction(actionSave);
    this->toolbars[tr("main")]->addAction(actionFind);
    this->toolbars[tr("main")]->addAction(actionReplace);
    this->toolbars[tr("main")]->addAction(actionGotoLine);
    this->toolbars[tr("main")]->addAction(actionFindPrev);
    this->toolbars[tr("main")]->addAction(actionFindNext);

    loadFile(fName);
    mdiClientName = getShortFileName();
}

qmdiEditor::~qmdiEditor() {
    bookmarksMenu->deleteLater();
    textOperationsMenu->deleteLater();
    mdiServer = nullptr;
}

QString qmdiEditor::getShortFileName() {
    if (getFileName().isEmpty()) {
        return tr("NO NAME");
    }

    // the name of the object for it's mdi server
    // is the file name alone, without the directory
    int i = getFileName().lastIndexOf('/');
    QString s;
    if (i != -1) {
        s = getFileName().mid(i + 1);
    } else {
        i = getFileName().lastIndexOf('\\');
        if (i != -1) {
            s = getFileName().mid(i + 1);
        } else {
            s = getFileName();
        }
    }
    return s;
}

bool qmdiEditor::canCloseClient() {
    if (!document()->isModified()) {
        return true;
    }

    QMessageBox msgBox(QMessageBox::Warning, tr("qtedit4"),
                       tr("The document has been modified.\nDo you want to save your changes?"),
                       QMessageBox::Yes | QMessageBox::Default, this);

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {
        return doSave();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

/**
 * \brief return the mdi file opened
 * \return the value of LinesEditor::getFileName()
 *
 * This function is override from qmdiClient::mdiClientFileName(),
 * and will tell the qmdiServer which file is opened by this mdi client.
 */
QString qmdiEditor::mdiClientFileName() { return getFileName(); }

void qmdiEditor::setupActions() {
    actionSave = new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
    actionSaveAs = new QAction(QIcon::fromTheme("document-save-as"), tr("&Save as..."), this);
    actionUndo = new QAction(QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
    actionRedo = new QAction(QIcon::fromTheme("edit-redo"), tr("&Redo"), this);
    actionCopy = new QAction(QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    actionCut = new QAction(QIcon::fromTheme("edit-cut"), tr("&Cut"), this);
    actionPaste = new QAction(QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    actionFind = new QAction(QIcon::fromTheme("edit-find"), tr("&Find"), this);
    actionFindNext = new QAction(QIcon::fromTheme("go-next"), tr("Find &next"), this);
    actionFindPrev = new QAction(QIcon::fromTheme("go-previous"), tr("Find &previous"), this);
    actionReplace = new QAction(QIcon::fromTheme("edit-find-replace"), tr("&Replace"), this);
    actionGotoLine = new QAction(QIcon::fromTheme("go-jump"), tr("&Goto line"), this);

    actionCapitalize = new QAction(tr("Change to &capital letters"), this);
    actionLowerCase = new QAction(tr("Change to &lower letters"), this);
    actionChangeCase = new QAction(tr("Change ca&se"), this);
    actionFindMatchingBracket = new QAction(tr("Find matching bracket"), this);

    actionSave->setShortcut(QKeySequence::Save);
    actionFind->setShortcut(QKeySequence::Find);
    actionFindNext->setShortcut(QKeySequence::FindNext);
    actionFindPrev->setShortcut(QKeySequence::FindPrevious);
    actionReplace->setShortcut(QKeySequence::Replace);
    actionGotoLine->setShortcut(QKeySequence("Ctrl+G"));
    actionCapitalize->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_U));
    actionLowerCase->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_U));
    actionFindMatchingBracket->setShortcuts(QList<QKeySequence>()
                                            << QKeySequence(Qt::CTRL | Qt::Key_6)
                                            << QKeySequence(Qt::CTRL | Qt::Key_BracketLeft)
                                            << QKeySequence(Qt::CTRL | Qt::Key_BracketRight));

    actionSave->setObjectName("qmdiEditor::actionSave");
    actionSaveAs->setObjectName("qmdiEditor::actionSaveAs");
    actionUndo->setObjectName("qmdiEditor::actionUndo");
    actionRedo->setObjectName("qmdiEditor::actionRedo");
    actionCopy->setObjectName("qmdiEditor::actionCopy");
    actionCut->setObjectName("qmdiEditor::actionCut");
    actionPaste->setObjectName("qmdiEditor::actionPaste");
    actionFind->setObjectName("qmdiEditor::actionFind");
    actionFindNext->setObjectName("qmdiEditor::actionFindNext");
    actionFindPrev->setObjectName("qmdiEditor::actionFindPrev");
    actionReplace->setObjectName("qmdiEditor::actionReplace");
    actionGotoLine->setObjectName("qmdiEditor::actionGotoLine");

    actionCapitalize->setObjectName("qsvEditor::actionCapitalize");
    actionLowerCase->setObjectName("qsvEditor::actionLowerCase");
    actionChangeCase->setObjectName("qsvEditor::actionChangeCase");
    actionFindMatchingBracket->setObjectName("qsvEditor::ctionFindMatchingBracket");

    connect(this, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(undoAvailable(bool)), actionUndo, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(redoAvailable(bool)), actionRedo, SLOT(setEnabled(bool)));

    connect(actionSave, SIGNAL(triggered()), this, SLOT(doSave()));
    connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(doSaveAs()));
    connect(actionUndo, SIGNAL(triggered()), this, SLOT(undo()));
    connect(actionRedo, SIGNAL(triggered()), this, SLOT(redo()));
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(actionCut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(actionPaste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(actionFind, SIGNAL(triggered()), operationsWidget, SLOT(showSearch()));
    connect(actionFindNext, SIGNAL(triggered()), operationsWidget, SLOT(searchNext()));
    connect(actionFindPrev, SIGNAL(triggered()), operationsWidget, SLOT(searchPrev()));
    connect(actionReplace, SIGNAL(triggered()), operationsWidget, SLOT(showReplace()));
    connect(actionGotoLine, SIGNAL(triggered()), operationsWidget, SLOT(showGotoLine()));

    connect(actionCapitalize, SIGNAL(triggered()), this, SLOT(transformBlockToUpper()));
    connect(actionLowerCase, SIGNAL(triggered()), this, SLOT(transformBlockToLower()));
    connect(actionChangeCase, SIGNAL(triggered()), this, SLOT(transformBlockCase()));
    connect(actionFindMatchingBracket, SIGNAL(triggered()), this, SLOT(gotoMatchingBracket()));
    // 	connect( actiohAskHelp, SIGNAL(triggered()), this, SLOT(helpShowHelp()));

    addAction(actionSave);
    addAction(actionSaveAs);
    addAction(actionUndo);
    addAction(actionRedo);
    addAction(actionCopy);
    addAction(actionCut);
    addAction(actionPaste);
    addAction(actionFind);
    addAction(actionFindNext);
    addAction(actionFindPrev);
    addAction(actionReplace);
    addAction(actionGotoLine);

    addAction(actionCapitalize);
    addAction(actionLowerCase);
    addAction(actionChangeCase);
    addAction(actionFindMatchingBracket);
}

void qmdiEditor::newDocument() { loadFile(""); }

bool qmdiEditor::doSave() {
    if (fileName.isEmpty()) {
        return doSaveAs();
    } else {
        return saveFile(fileName);
    }
}

bool qmdiEditor::doSaveAs() {
    const QString lastDirectory;
    QString s = QFileDialog::getSaveFileName(this, tr("Save file"), lastDirectory);
    if (s.isEmpty()) {
        return false;
    }
    return saveFile(s);
}

bool qmdiEditor::loadFile(const QString &fileName) {
    // clear older watches, and add a new one
    // QStringList sl = m_fileSystemWatcher->directories();
    // if (!sl.isEmpty()) {
    //     m_fileSystemWatcher->removePaths(sl);
    // }

    // bool modificationsEnabledState = getModificationsLookupEnabled();
    // setModificationsLookupEnabled(false);
    // hideBannerMessage();
    this->setReadOnly(false);
    // QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // QApplication::processEvents();
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        QFileInfo fileInfo(file);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QApplication::restoreOverrideCursor();
            return false;
        }

        QTextStream textStream(&file);
        setPlainText(textStream.readAll());
        file.close();

        auto langInfo = ::Qutepart::chooseLanguage(QString(), QString(), fileName);
        if (langInfo.isValid()) {
            setHighlighter(langInfo.id);
            setIndentAlgorithm(langInfo.indentAlg);
        }

        this->fileName = fileInfo.absoluteFilePath();
        // m_fileSystemWatcher->addPath(m_fileName);
        // if (!fileInfo.isWritable()) {
        //     this->setReadOnly(true);
        //     displayBannerMessage(
        //         tr("The file is readonly. Click <a href=':forcerw' title='Click here to try and "
        //            "change the file attributes for write access'>here to force write
        //            access.</a>"));
        // }
    } else {
        this->fileName.clear();
        clear();
    }

    // setModificationsLookupEnabled(modificationsEnabledState);
    // removeModifications();

    // QApplication::restoreOverrideCursor();
    return true;
}

bool qmdiEditor::saveFile(const QString &fileName) {
    // QStringList sl = m_fileSystemWatcher->directories();
    // if (!sl.isEmpty()) {
    //     m_fileSystemWatcher->removePaths(sl);
    // }
    // bool modificationsEnabledState = getModificationsLookupEnabled();
    // setModificationsLookupEnabled(false);
    // hideBannerMessage();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QApplication::processEvents();

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QTextStream textStream(&file);
    QTextBlock block = document()->begin();
    while (block.isValid()) {
        //		if (endOfLine==KeepOldStyle){
        textStream << block.text();
        // TODO WTF? - which type the file originally had?
        textStream << "\n";
        /*		} else {
                                QString s = block.text();
                                int i = s.length();

                                if (!s.isEmpty()) if ((s[i-1] == '\n') || (s[i-1] == '\r'))
                                        s = s.left( i-1 );
                                if (!s.isEmpty()) if ((s[i-1] == '\n') || (s[i-1] == '\r'))
                                        s = s.left( i-1 );
                                textStream << s;
                                switch (endOfLine) {
                                        case DOS:	textStream << "\r\n"; break;
                                        case Unix: 	textStream << "\n"; break;
                                        case Mac:	textStream << "\r"; break;
                                        default:	return 0; // just to keep gcc happy
                                }
                        }*/
        block = block.next();
    }
    file.close();

    this->fileName = fileName;
    this->mdiClientName = getShortFileName();
    // removeModifications();
    // setModificationsLookupEnabled(modificationsEnabledState);
    // m_fileSystemWatcher->addPath(m_fileName);

    QApplication::restoreOverrideCursor();

    auto w = dynamic_cast<QTabWidget *>(this->mdiServer);
    auto i = w->indexOf(this);
    w->setTabText(i, mdiClientName);
    w->setTabToolTip(i, mdiClientFileName());
    return true;
}

// void QsvTextEdit::displayBannerMessage(QString message, int time) {
//     showUpperWidget(m_banner);
//     ui_banner->label->setText(message);
//     m_timerHideout = time;
//     QTimer::singleShot(1000, this, SLOT(on_hideTimer_timeout()));
// }

// void QsvTextEdit::hideBannerMessage() {
//     m_timerHideout = 0;
//     ui_banner->label->clear();
//     m_banner->hide();

//     // sometimes the top widget is displayed, lets workaround this
//     // TODO: find WTF this is happening
//     if (m_topWidget == m_banner) {
//         m_topWidget = nullptr;
//     }
// }

void qmdiEditor::smartHome() {
    auto c = textCursor();
    int blockLen = c.block().text().length();
    if (blockLen == 0) {
        return;
    }

    int originalPosition = c.position();
    QTextCursor::MoveMode moveAnchor = QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier)
                                           ? QTextCursor::KeepAnchor
                                           : QTextCursor::MoveAnchor;
    c.movePosition(QTextCursor::StartOfLine, moveAnchor);
    int startOfLine = c.position();
    int i = 0;
    while (c.block().text()[i].isSpace()) {
        i++;
        if (i == blockLen) {
            i = 0;
            break;
        }
    }
    if ((originalPosition == startOfLine) || (startOfLine + i != originalPosition)) {
        c.setPosition(startOfLine + i, moveAnchor);
    }
    setTextCursor(c);
}

void qmdiEditor::smartEnd() {
    QTextCursor c = textCursor();
    int blockLen = c.block().text().length();
    if (blockLen == 0) {
        return;
    }

    int originalPosition = c.position();
    QTextCursor::MoveMode moveAnchor = QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier)
                                           ? QTextCursor::KeepAnchor
                                           : QTextCursor::MoveAnchor;
    c.movePosition(QTextCursor::StartOfLine, moveAnchor);
    int startOfLine = c.position();
    c.movePosition(QTextCursor::EndOfLine, moveAnchor);
    int i = blockLen;
    while (c.block().text()[i - 1].isSpace()) {
        i--;
        if (i == 1) {
            i = blockLen;
            break;
        }
    }
    if ((originalPosition == startOfLine) || (startOfLine + i != originalPosition)) {
        c.setPosition(startOfLine + i, moveAnchor);
    }

    setTextCursor(c);
}

void qmdiEditor::transformBlockToUpper() {
    QTextCursor cursor = textCursor();
    QString s_before = cursor.selectedText();
    QString s_after = s_before.toUpper();

    if (s_before != s_after) {
        cursor.beginEditBlock();
        cursor.deleteChar();
        cursor.insertText(s_after);
        cursor.endEditBlock();
        setTextCursor(cursor);
    }
}

void qmdiEditor::transformBlockToLower() {
    QTextCursor cursor = textCursor();
    QString s_before = cursor.selectedText();
    QString s_after = s_before.toLower();

    if (s_before != s_after) {
        cursor.beginEditBlock();
        cursor.deleteChar();
        cursor.insertText(s_after);
        cursor.endEditBlock();
        setTextCursor(cursor);
    }
}

void qmdiEditor::transformBlockCase() {
    QTextCursor cursor = textCursor();
    QString s_before = cursor.selectedText();
    QString s_after = s_before;
    int s_len = s_before.length();

    for (int i = 0; i < s_len; i++) {
        QChar c = s_after[i];
        if (c.isLower()) {
            c = c.toUpper();
        } else if (c.isUpper()) {
            c = c.toLower();
        }
        s_after[i] = c;
    }

    if (s_before != s_after) {
        cursor.beginEditBlock();
        cursor.deleteChar();
        cursor.insertText(s_after);
        cursor.endEditBlock();
        setTextCursor(cursor);
    }
}

void qmdiEditor::gotoMatchingBracket() {
#if 0
    /*
      WARNING: code duplication between this method and on_cursor_positionChanged();
      this needs to be refactored
     */

    QTextCursor cursor = textCursor();
    QTextBlock block = cursor.block();
    int blockPosition = block.position();
    int cursorPosition = cursor.position();
    int relativePosition = cursorPosition - blockPosition;
    auto textBlock = block.text();

    if (relativePosition == textBlock.size()) {
        relativePosition--;
    }

    QChar currentChar = textBlock[relativePosition];

    // lets find it's partner
    // in theory, no errors should not happen, but one can never be too sure
    int j = m_config.matchBracketsList.indexOf(currentChar);
    if (j == -1) {
        return;
    }

    if (m_config.matchBracketsList[j] != m_config.matchBracketsList[j + 1]) {
        if (j % 2 == 0) {
            j = findMatchingChar(m_config.matchBracketsList[j], m_config.matchBracketsList[j + 1],
                                 true, block, cursorPosition);
        } else {
            j = findMatchingChar(m_config.matchBracketsList[j], m_config.matchBracketsList[j - 1],
                                 false, block, cursorPosition);
        }
    } else {
        j = findMatchingChar(m_config.matchBracketsList[j], m_config.matchBracketsList[j + 1], true,
                             block, cursorPosition);
    }

    cursor.setPosition(j);
    setTextCursor(cursor);
#endif
}

bool qmdiEditor::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Resize) {
        emit widgetResized();
    }
    return Qutepart::eventFilter(obj, event);
}
