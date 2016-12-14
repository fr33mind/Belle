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
    QList<Object*> objects() const;
    void setCurrentObject(Object*);
    Object* currentObject() const;
    Object* objectAt(int) const;
    Object* object(const QString&) const;
    void removeObject(Object*);

    void addTypeFilter(GameObjectMetaType::Type);
    QList<GameObjectMetaType::Type> typeFilters() const;
    void clearTypeFilters();

protected:
    void setupConnections();
    virtual void wheelEvent(QWheelEvent *);

signals:
    void objectChanged(Object*);
    void objectChanged(const QString&);
    void objectHighlighted(Object*);

private slots:
    void indexChanged(int);
    void objectDestroyed(Object*);
    void onHighlighted(int);
    void onEditTextChanged(const QString&);

private:
    QList<GameObjectMetaType::Type> mTypeFilters;

};

#endif // OBJECT_COMBOBOX_H
