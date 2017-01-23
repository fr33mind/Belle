#ifndef ACTIONMETATYPE_H
#define ACTIONMETATYPE_H

#include <QObject>
#include <QColor>

#include "gameobjectmetatype.h"
#include "action.h"

class Action;

class ActionMetaType : public QObject
{
    Q_OBJECT
public:
    explicit ActionMetaType(QObject *parent = 0);
    static void init();
    static QColor color(GameObjectMetaType::Type);
    static QColor color(const Action*);

signals:

public slots:

};

#endif // ACTIONMETATYPE_H
