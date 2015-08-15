#include "runscript_editorwidget.h"

RunScriptEditorWidget::RunScriptEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mScriptEdit = new QTextEdit(this);
    QFont font;
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);
    mScriptEdit->setFont(font);
    QFontMetrics metrics(font);
    mScriptEdit->setTabStopWidth(TAB_WIDTH * metrics.width(' '));

    mCommentEdit = new QLineEdit(this);

    this->appendRow(tr("Script"), mScriptEdit);
    this->appendRow(tr("Comment"), mCommentEdit);
    this->resizeColumnToContents(0);

    connect(mScriptEdit, SIGNAL(textChanged()), this, SLOT(onScriptTextChanged()));
    connect(mCommentEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onCommentTextEdited(const QString&)));

}

void RunScriptEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    RunScript* runScript = qobject_cast<RunScript*>(action);
    if (! runScript)
        return;

    mScriptEdit->setPlainText(runScript->script());
    mCommentEdit->setText(runScript->comment());
}

void RunScriptEditorWidget::onScriptTextChanged()
{
    RunScript* runScript = qobject_cast<RunScript*>(mGameObject);
    if (runScript)
        runScript->setScript(mScriptEdit->toPlainText());
}

void RunScriptEditorWidget::onCommentTextEdited(const QString&)
{
    RunScript* runScript = qobject_cast<RunScript*>(mGameObject);
    if (runScript)
        runScript->setComment(mCommentEdit->text());
}
