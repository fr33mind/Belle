#ifndef STOPALLSOUNDSEDITORWIDGET_H
#define STOPALLSOUNDSEDITORWIDGET_H

#include <QDoubleSpinBox>

#include "action_editor_widget.h"

class StopAllSoundsEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    QDoubleSpinBox* mFadeOutSpinBox;

public:
    explicit StopAllSoundsEditorWidget(QWidget *parent = 0);

protected:
    virtual void updateData(GameObject*);

signals:

public slots:

private slots:
    void onFadeChanged(double);

};

#endif // STOPALLSOUNDSEDITORWIDGET_H
