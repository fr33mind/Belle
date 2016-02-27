#include "gameobject.h"

#include "scene.h"

GameObject::GameObject(QObject *parent) :
    QObject(parent)
{
    init();
}

GameObject::GameObject(const QVariantMap & data, QObject *parent) :
    QObject(parent)
{
    init();
    _load(data);
}

GameObject::~GameObject()
{
    emit destroyed(this);
}

void GameObject::init()
{
    mNameEditable = true;
    mResource = 0;
    mSynced = true;
    mType = "GameObject";
    mManager = 0;
}

void GameObject::_load(const QVariantMap & data)
{
    if (data.contains("name") && data.value("name").type() == QVariant::String)
        setName(data.value("name").toString());

    if (data.contains("type") && data.value("type").type() == QVariant::String)
        setType(data.value("type").toString());
}

void GameObject::load(const QVariantMap & data)
{
    QVariantMap _data = data;
    _data.remove("name");
    _load(_data);
}

QVariantMap GameObject::toJsonObject(bool internal) const
{
    QVariantMap data;
    if (! name().isEmpty())
        data.insert("name", name());
    data.insert("type", mType);
    return data;
}

void GameObject::setType(const QString & type)
{
    mType = type;
}

QString GameObject::type() const
{
    return mType;
}

void GameObject::setName(const QString & name)
{
    if (mManager && !mManager->isValidName(name))
        return;

    setObjectName(name);
    notify("name", name);
}

QString GameObject::name() const
{
    return objectName();
}

void GameObject::setNameEditable(bool editable)
{
    mNameEditable = editable;
}

bool GameObject::nameEditable() const
{
    return mNameEditable;
}

void GameObject::setResource(GameObject * resource)
{
    if (! resource || resource == mResource)
        return;

    mResource = resource;
    mResource->addClone(this);
    connectToResource();
}

GameObject* GameObject::resource() const
{
    return mResource;
}

void GameObject::resourceDestroyed()
{
    mResource = 0;
}

void GameObject::addClone(GameObject * clone)
{
    if (! clone || hasClone(clone))
        return;

    mClones.append(clone);
}

bool GameObject::hasClone(GameObject * clone)
{
    return mClones.contains(clone);
}

void GameObject::removeClone(GameObject * clone)
{
    if (clone && hasClone(clone)) {
        disconnect(clone);
        clone->disconnect(this);
        mClones.removeOne(clone);
    }
}

QList<GameObject*> GameObject::clones() const
{
    return mClones;
}

void GameObject::blockNotifications(bool block)
{
    // don't block signals for resources
    if (block && mClones.size())
        return;

    this->blockSignals(block);
}

void GameObject::setSync(bool sync)
{
    if (! mResource || sync == mSynced)
        return;

    mSynced = sync;

    if (sync)
        connectToResource();
    else
        disconnectFromResource();
}

void GameObject::sync()
{
    load(mResource->toJsonObject());
}

bool GameObject::isSynced()
{
    return mSynced;
}

void GameObject::connectToResource()
{
    if (mResource) {
        if (mSynced) {
            connect(mResource, SIGNAL(dataChanged(const QVariantMap&)), this, SLOT(load(const QVariantMap&)), Qt::UniqueConnection);
            connect(this, SIGNAL(dataChanged(const QVariantMap&)), mResource, SLOT(load(const QVariantMap&)), Qt::UniqueConnection);
        }

        connect(mResource, SIGNAL(destroyed()), this, SLOT(resourceDestroyed()), Qt::UniqueConnection);
        connect(this, SIGNAL(destroyed(GameObject*)), mResource, SLOT(removeClone(GameObject*)), Qt::UniqueConnection);
    }
}

void GameObject::disconnectFromResource()
{
    if (mResource) {
        disconnect(mResource);
        mResource->disconnect(this);
    }
}

Scene * GameObject::scene() const
{
    if (! this->parent())
        return 0;

    Scene* scene = qobject_cast<Scene*>(this->parent());
    if (scene)
        return scene;

    GameObject* object = qobject_cast<GameObject*>(this->parent());
    if (object)
        return object->scene();

    return 0;
}

void GameObject::notify(const QString & property, const QVariant & value)
{
    QVariantMap data;
    data.insert(property, value);
    emit dataChanged(data);
}

GameObjectManager* GameObject::manager() const
{
    return mManager;
}

void GameObject::setManager(GameObjectManager* manager)
{
    mManager = manager;
}

