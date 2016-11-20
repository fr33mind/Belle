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

    QString name() const;

    void setNameEditable(bool);
    bool nameEditable() const;

    void setResource(GameObject*);
    GameObject* resource() const;

    void addClone(GameObject*);
    bool hasClone(GameObject*);
    QList<GameObject*> clones() const;
    bool isResource() const;

    bool blockLoad(bool);
    bool loadBlocked() const;

    void sync();
    bool isSynced() const;

    Scene* scene() const;

    GameObjectManager* manager() const;
    void setManager(GameObjectManager*);

public slots:
    void load(const QVariantMap&);
    void removeClone(GameObject*);
    bool setName(const QString&);
    void setSync(bool);

private slots:
    void resourceDestroyed();

protected:
    virtual void connectToResource();
    virtual void disconnectFromResource();
    virtual void filterLoadData(QVariantMap&);
    virtual void beforeLoadData(const QVariantMap&);
    virtual void loadData(const QVariantMap&, bool internal=false);
    virtual void afterLoadData(const QVariantMap&);
    void notify(const QString&, const QVariant&);
    void notify(const QVariantMap&);
    void blockNotifications(bool);
    void setType(GameObjectMetaType::Type);
    void loadInternal(const QVariantMap&);

private:
    void init();

private:
    GameObject* mResource;
    bool mNameEditable;
    bool mSynced;
    GameObjectMetaType::Type mType;
    QList<GameObject*> mClones;
    GameObjectManager* mManager;
    bool mLoadBlocked;

signals:
    void destroyed(GameObject*);
    void dataChanged(const QVariantMap&data=QVariantMap());
    void nameChanged(const QString&);
    void syncChanged(bool);

public slots:

};

#endif // GAMEOBJECT_H
