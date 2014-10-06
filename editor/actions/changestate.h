#ifndef CHANGESTATE_H
#define CHANGESTATE_H

#include "action.h"
#include "changestate_editorwidget.h"

class ChangeStateEditorWidget;

class ChangeState : public Action
{
    Q_OBJECT

public:
    static ActionInfo Info;

public:
    explicit ChangeState(QObject *parent = 0);
    ChangeState(const QVariantMap&, QObject *parent = 0);
    static ChangeStateEditorWidget* changeStateEditorWidget();
    static void setChangeStateEditorWidget(ChangeStateEditorWidget*);
    virtual ActionEditorWidget* editorWidget();
    virtual QString displayText() const;
    QList<Object*> availableObjects() const;

    virtual QVariantMap toJsonObject();

    QString state() const;
    void setState(const QString&);


signals:

public slots:

private:
    void init();
    QString mState;

};

#endif // CHANGESTATE_H
