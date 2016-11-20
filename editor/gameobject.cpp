#include "gameobject.h"

#include "scene.h"
#include "resource_manager.h"

GameObject::GameObject(QObject *parent, const QString& name) :
    QObject(parent)
{
    init();
    setName(name);
}

GameObject::GameObject(const QVariantMap & data, QObject *parent) :
    QObject(parent)
{
    init();
    loadInternal(data);
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
    mType = GameObjectMetaType::GameObject;
    mManager = 0;
    mLoadBlocked = false;
}

void GameObject::load(const QVariantMap & data)
{
    if (mLoadBlocked || data.isEmpty())
        return;

    QVariantMap _data = data;
    filterLoadData(_data);
    if (_data.isEmpty())
        return;

    blockNotifications(true);
    beforeLoadData(_data);
    loadData(_data);
    afterLoadData(_data);
    blockNotifications(false);
}

void GameObject::loadInternal(const QVariantMap & data)
{
    if (mLoadBlocked || data.isEmpty())
        return;

    blockNotifications(true);
    beforeLoadData(data);
    loadData(data, true);
    afterLoadData(data);
    blockNotifications(false);
}

void GameObject::filterLoadData(QVariantMap & data)
{
    data.remove("name");
    data.remove("sync");
}

void GameObject::loadData(const QVariantMap & data, bool internal)
{
    if (data.contains("name") && data.value("name").type() == QVariant::String)
        setName(data.value("name").toString());

    if (data.contains("type") && data.value("type").type() == QVariant::String)
        setType(GameObjectMetaType::typeFromString(data.value("type").toString()));

    if (data.contains("sync") && data.value("sync").type() == QVariant::Bool)
        setSync(data.value("sync").toBool());
}

void GameObject::beforeLoadData(const QVariantMap & data)
{
}

void GameObject::afterLoadData(const QVariantMap & data)
{
}

QVariantMap GameObject::toJsonObject(bool internal) const
{
    QVariantMap data;
    if (! name().isEmpty())
        data.insert("name", name());
    const GameObjectMetaType* metatype = GameObjectMetaType::metaType(type());
    if (metatype)
        data.insert("type", metatype->toString());
    data.insert("sync", mSynced);
    return data;
}

void GameObject::setType(GameObjectMetaType::Type type)
{
    mType = type;
}

GameObjectMetaType::Type GameObject::type() const
{
    return mType;
}

bool GameObject::setName(const QString & name)
{
    if (this->name() == name)
        return true;

    if (mManager && !mManager->isValidName(this, name))
        return false;

    setObjectName(name);
    emit nameChanged(name);
    return true;
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
    if (resource == mResource)
        return;

    disconnectFromResource();
    mResource = resource;

    if (mResource) {
        mResource->addClone(this);
        connect(mResource, SIGNAL(destroyed()), this, SLOT(resourceDestroyed()), Qt::UniqueConnection);
        connect(this, SIGNAL(destroyed(GameObject*)), mResource, SLOT(removeClone(GameObject*)), Qt::UniqueConnection);
        if (mSynced)
            connectToResource();
    }
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
    if (sync == mSynced)
        return;

    mSynced = sync;

    if (sync)
        connectToResource();
    else
        disconnectFromResource();

    emit syncChanged(sync);
}

void GameObject::sync()
{
    if (mResource)
        load(mResource->toJsonObject());
}

bool GameObject::isSynced() const
{
    return mSynced;
}

void GameObject::connectToResource()
{
    if (mResource) {
        connect(mResource, SIGNAL(dataChanged(const QVariantMap&)), this, SLOT(load(const QVariantMap&)), Qt::UniqueConnection);
        connect(this, SIGNAL(dataChanged(const QVariantMap&)), mResource, SLOT(load(const QVariantMap&)), Qt::UniqueConnection);
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
    notify(data);
}

void GameObject::notify(const QVariantMap & data)
{
    bool loadBlocked = blockLoad(true);
    emit dataChanged(data);
    blockLoad(loadBlocked);
}

GameObjectManager* GameObject::manager() const
{
    return mManager;
}

void GameObject::setManager(GameObjectManager* manager)
{
    mManager = manager;
}

bool GameObject::isResource() const
{
    QObject* parent = this->parent();

    if (parent == ResourceManager::instance())
        return true;

    GameObject* _parent = qobject_cast<GameObject*>(parent);
    if (_parent && _parent->isResource())
        return true;

    return false;
}

bool GameObject::blockLoad(bool block)
{
    bool oldBlock = mLoadBlocked;
    mLoadBlocked = block;
    return oldBlock;
}

bool GameObject::loadBlocked() const
{
    return mLoadBlocked;
}
