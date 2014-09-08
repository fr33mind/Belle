#ifndef CHANGESTATE_EDITORWIDGET_H
#define CHANGESTATE_EDITORWIDGET_H

#include "action_editor_widget.h"
#include "changestate.h"

class ChangeState;

class ChangeStateEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

public:
    explicit ChangeStateEditorWidget(QWidget *parent = 0);
    virtual void updateData(Action*);

private:
    ChangeState* mChangeStateAction;
    QComboBox* mCharacterBox;
    QComboBox* mStateBox;

signals:

public slots:
    void onObjectChosen(const QString&);
    void onStateChosen(const QString&);

};

#endif // CHANGESTATE_EDITORWIDGET_H
