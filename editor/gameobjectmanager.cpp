#include "gameobjectmanager.h"

#include "utils.h"

GameObjectManager::GameObjectManager(QObject *parent) :
    QObject(parent)
{
    setUniqueNames(true);
    setAllowEmptyNames(false);
    mObjectsParent = this;
    mTakeObjectsOwnership = true;
}

GameObjectManager::~GameObjectManager()
{
}

void GameObjectManager::prepareObject(GameObject * object)
{
    if (! object)
        return;

    connect(object, SIGNAL(destroyed(GameObject*)),
            this, SLOT(onObjectDestroyed(GameObject*)), Qt::UniqueConnection);

    if (! hasValidName(object))
        renameObject(object);

    object->setManager(this);
    if (mTakeObjectsOwnership)
        object->setParent(mObjectsParent);
}

void GameObjectManager::add(GameObject* object)
{
    insert(size(), object);
    emit objectAdded(object);
}

int GameObjectManager::indexOf(GameObject* obj) const
{
    return mGameObjects.indexOf(obj);
}

GameObject* GameObjectManager::objectAt(int index) const
{
    if (index >= 0 && index < mGameObjects.size())
        return mGameObjects.at(index);
    return 0;
}

QList<GameObject*> GameObjectManager::objects() const
{
    return mGameObjects;
}

QList<GameObject*> GameObjectManager::objects(GameObjectMetaType::Type type) const
{
    QList<GameObject*> objects;

    foreach(GameObject* obj, mGameObjects) {
        if (obj->type() == type)
            objects.append(obj);
    }

    return objects;
}

GameObject* GameObjectManager::takeAt(int index)
{
    if (index < 0 || index >= mGameObjects.size())
        return 0;

    GameObject* obj = mGameObjects.takeAt(index);
    if (obj)
        obj->disconnect(this);
    emit objectTaken(obj);
    return obj;
}

void GameObjectManager::insert(int index, GameObject* object)
{
    if (contains(object))
        return;

    if (index < 0)
        index = 0;
    else if (index > mGameObjects.size())
        index = mGameObjects.size();

    prepareObject(object);
    mGameObjects.insert(index, object);
    emit objectInserted(index, object);
}

void GameObjectManager::removeAt(int index, bool del)
{
    if (index >= 0 && index < mGameObjects.size()) {
        GameObject* obj = takeAt(index);
        emit objectRemoved(obj, del);
        if (del && obj)
            delete obj;
    }
}

bool GameObjectManager::remove(GameObject* obj, bool del)
{
    if (! obj)
        return false;

    int index = indexOf(obj);
    if (index != -1) {
        removeAt(index, del);
        return true;
    }

    return false;
}

bool GameObjectManager::move(GameObject* obj, int to)
{
    int from = indexOf(obj);
    if (from == -1 || from == to)
        return false;

    bool blocked = blockSignals(true);
    removeAt(from);
    insert(to, obj);
    blockSignals(blocked);
    emit objectMoved(obj, to);
    return true;
}

bool GameObjectManager::contains(GameObject* obj) const
{
    return mGameObjects.contains(obj);
}

bool GameObjectManager::contains(const QString& name) const
{
    GameObject* obj = this->object(name);
    if (obj)
        return true;
    return false;
}

int GameObjectManager::count() const
{
    return mGameObjects.size();
}

int GameObjectManager::size() const
{
    return mGameObjects.size();
}

void GameObjectManager::clear(bool del)
{
    for(int i=mGameObjects.size()-1; i >= 0; --i) {
        removeAt(i, del);
    }

    mGameObjects.clear();
}

GameObject* GameObjectManager::object(const QString& name) const
{
    for(int i=0; i < mGameObjects.size(); i++)
        if (mGameObjects[i]->name() == name)
            return mGameObjects[i];
    return 0;
}

void GameObjectManager::setUniqueNames(bool unique)
{
    mUniqueNames = unique;
    if (unique) {
        renameObjects();
    }
}

bool GameObjectManager::hasUniqueNames() const
{
    return mUniqueNames;
}

void GameObjectManager::setAllowEmptyNames(bool empty)
{
    mAllowEmptyNames = empty;
}

bool GameObjectManager::allowEmptyNames() const
{
    return mAllowEmptyNames;
}

bool GameObjectManager::isValidName(GameObject* object, const QString& name) const
{
    if (mAllowEmptyNames && name.isEmpty())
        return true;
    else if (name.isEmpty())
        return false;

    if (hasUniqueNames()) {
        for(int i=0; i < mGameObjects.size(); i++)
            if (mGameObjects.at(i) != object && mGameObjects.at(i)->objectName() == name)
                return false;
    }

    return true;
}

bool GameObjectManager::isValidName(const QString& name) const
{
    return isValidName(0, name);
}

bool GameObjectManager::hasValidName(GameObject* object) const
{
    if (object)
        return isValidName(object, object->name());
    return false;
}

QString GameObjectManager::uniqueName(GameObject* object, const QString& newName)
{
    if (! object)
        return newName;

    QString name = newName;

    if (name.isEmpty())
        name = object->name();
    if (name.isEmpty()) {
        const GameObjectMetaType* metatype = GameObjectMetaType::metaType(object->type());
        name = metatype ? metatype->toString() : "";
    }

    while(! isValidName(object, name)) {
        name = Utils::incrementLastNumber(name);
    }

    return name;
}

void GameObjectManager::renameObject(GameObject* object, const QString& newName)
{
    if (!object)
        return;

    QString name = newName;
    name = uniqueName(object, name);
    object->setName(name);
}

void GameObjectManager::renameObjects()
{
    for(int i=0; i < mGameObjects.size(); i++) {
        renameObject(mGameObjects.at(i));
    }
}

void GameObjectManager::onObjectDestroyed(GameObject* obj)
{
    remove(obj);
}

QObject* GameObjectManager::objectsParent() const
{
    return mObjectsParent;
}

void GameObjectManager::setObjectsParent(QObject * parent)
{
    mObjectsParent = parent;
}

bool GameObjectManager::takeObjectsOwnership() const
{
    return mTakeObjectsOwnership;
}

void GameObjectManager::setTakeObjectsOwnership(bool take)
{
    mTakeObjectsOwnership = take;
}
