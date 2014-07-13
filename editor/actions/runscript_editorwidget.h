#ifndef RUNSCRIPT_EDITORWIDGET_H
#define RUNSCRIPT_EDITORWIDGET_H

#include "action_editor_widget.h"
#include "runscript.h"

#include <QTextEdit>
#include <QLineEdit>

#define TAB_WIDTH 4

class ActionEditorWidget;
class RunScript;

class RunScriptEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    QTextEdit* mScriptEdit;
    QLineEdit* mCommentEdit;

public:
    explicit RunScriptEditorWidget(QWidget *parent = 0);
    virtual void updateData(Action *);

signals:
    
public slots:
    void onScriptTextChanged();
    void onCommentTextEdited(const QString&);
};

#endif // RUNSCRIPT_EDITORWIDGET_H
