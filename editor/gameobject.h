#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>
#include <QVariantMap>

#include "gameobjectmetatype.h"

class Scene;
class GameObjectManager;

class GameObject : public QObject
{
    Q_OBJECT

public:
    explicit GameObject(QObject *parent = 0, const QString& name="");
    GameObject(const QVariantMap&, QObject *parent = 0);
    virtual ~GameObject();

    virtual QVariantMap toJsonObject(bool internal=true) const;

    GameObjectMetaType::Type type() const;

    bool setName(const QString&);
    QString name() const;

    void setNameEditable(bool);
    bool nameEditable() const;

    void setResource(GameObject*);
    GameObject* resource() const;

    void addClone(GameObject*);
    bool hasClone(GameObject*);
    QList<GameObject*> clones() const;

    void sync();
    void setSync(bool);
    bool isSynced() const;

    Scene* scene() const;

    GameObjectManager* manager() const;
    void setManager(GameObjectManager*);

public slots:
    virtual void load(const QVariantMap&);
    void removeClone(GameObject*);

private slots:
    void resourceDestroyed();

protected:
    virtual void connectToResource();
    virtual void disconnectFromResource();
    void notify(const QString&, const QVariant&);
    void blockNotifications(bool);
    void setType(GameObjectMetaType::Type);

private:
    void init();
    void _load(const QVariantMap&);

private:
    GameObject* mResource;
    bool mNameEditable;
    bool mSynced;
    GameObjectMetaType::Type mType;
    QList<GameObject*> mClones;
    GameObjectManager* mManager;

signals:
    void destroyed(GameObject*);
    void dataChanged(const QVariantMap&data=QVariantMap());
    void nameChanged(const QString&);

public slots:

};

#endif // GAMEOBJECT_H
