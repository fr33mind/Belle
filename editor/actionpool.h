#ifndef ACTIONPOOL_H
#define ACTIONPOOL_H

#include <QObject>

#include "action.h"

class Action;

class ActionPool : public QObject
{
    Q_OBJECT
public:
    explicit ActionPool(QObject *parent = 0);
    virtual ~ActionPool();

    QList<Action*> actions() const;
    void addAction(Action*);
    bool removeAction(Action*);
    bool contains(Action*) const;
    Action* actionFromParent(GameObject*) const;
    int size() const;

signals:
    void destroyed(ActionPool*);

private slots:
    void onActionDestroyed(QObject*);

private:
    QList<Action*> mActions;

};

#endif // ACTIONPOOL_H
