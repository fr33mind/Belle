#ifndef OBJECT_COMBOBOX_H
#define OBJECT_COMBOBOX_H

#include <QComboBox>

#include "scene.h"
#include "action.h"
#include "object.h"

class Scene;
class Action;
class Object;

class ObjectComboBox : public QComboBox
{
    Q_OBJECT

    QList<Object*> mObjects;

public:
    explicit ObjectComboBox(QWidget *parent = 0);
    void loadFromAction(Action*);
    void loadFromScene(Scene*);
    void setObjects(const QList<Object*>&);
    void insertObject(int, Object*);
    void addObject(Object*);
    void clear();
    Object* currentObject() const;
    void removeObject(Object*);

protected:
    void setupConnections();

signals:
    void objectChanged(Object*);

private slots:
    void indexChanged(int);
    void objectDestroyed(Object*);

};

#endif // OBJECT_COMBOBOX_H
