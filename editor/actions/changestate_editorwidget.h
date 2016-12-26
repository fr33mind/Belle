#ifndef CHANGESTATE_EDITORWIDGET_H
#define CHANGESTATE_EDITORWIDGET_H

#include "action_editor_widget.h"
#include "changestate.h"
#include "object_combobox.h"

class ChangeState;

class ChangeStateEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

public:
    explicit ChangeStateEditorWidget(QWidget *parent = 0);

private:
    ObjectComboBox* mCharacterBox;
    QComboBox* mStateBox;

signals:

protected:
    virtual void updateData(GameObject*);

private slots:
    void onObjectChosen(Object*);
    void onStateChosen(const QString&);

};

#endif // CHANGESTATE_EDITORWIDGET_H
