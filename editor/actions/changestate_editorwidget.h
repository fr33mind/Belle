#ifndef CHANGESTATE_EDITORWIDGET_H
#define CHANGESTATE_EDITORWIDGET_H

#include <QComboBox>

#include "action_editor_widget.h"
#include "changestate.h"

class ChangeState;

class ChangeStateEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

public:
    explicit ChangeStateEditorWidget(QWidget *parent = 0);
    virtual void setGameObject(GameObject*);

private:
    QComboBox* mCharacterBox;
    QComboBox* mStateBox;

signals:

protected:
    virtual void updateData(GameObject*);

private slots:
    void onObjectChosen(const QString&);
    void onStateChosen(const QString&);

};

#endif // CHANGESTATE_EDITORWIDGET_H
