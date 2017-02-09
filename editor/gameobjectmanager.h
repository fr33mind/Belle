#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H

#include <QObject>

#include "gameobject.h"

class GameObjectManager : public QObject
{
    Q_OBJECT

public:
    explicit GameObjectManager(QObject *parent = 0);
    virtual ~GameObjectManager();

    void add(GameObject*);
    int indexOf(GameObject*) const;
    GameObject* objectAt(int) const;
    GameObject* object(const QString&) const;
    QList<GameObject*> objects() const;
    QList<GameObject*> objects(GameObjectMetaType::Type) const;
    GameObject* takeAt(int);
    void insert(int, GameObject*);
    bool remove(GameObject*, bool del=false);
    void removeAt(int, bool del=false);
    bool move(GameObject*, int);
    bool contains(GameObject*) const;
    bool contains(const QString&) const;
    void clear(bool del=false);
    int count() const;
    int size() const;
    bool isValidName(GameObject*, const QString&) const;
    bool isValidName(const QString&) const;
    bool hasValidName(GameObject*) const;
    QString uniqueName(GameObject*, const QString& newName="");

    void setUniqueNames(bool);
    bool hasUniqueNames() const;

    void setAllowEmptyNames(bool);
    bool allowEmptyNames() const;

    QObject* objectsParent() const;
    void setObjectsParent(QObject*);

private:
    void prepareObject(GameObject*);

protected:
    void renameObject(GameObject*, const QString& newName="");
    void renameObjects();

private:
    QList<GameObject*> mGameObjects;
    bool mUniqueNames;
    bool mAllowEmptyNames;
    QObject* mObjectsParent;

signals:
    void objectAdded(GameObject*);
    void objectInserted(int, GameObject*);
    void objectTaken(GameObject*);
    void objectRemoved(GameObject*, bool del=false);
    void objectChanged();
    void objectMoved(GameObject*, int);

private slots:
    void onObjectDestroyed(GameObject*);

};

#endif // GAMEOBJECTMANAGER_H
