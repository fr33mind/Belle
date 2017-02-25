#include "object_combobox.h"

ObjectComboBox::ObjectComboBox(QWidget *parent) :
    QComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
    connect(this, SIGNAL(highlighted(int)), this, SLOT(onHighlighted(int)));
    connect(this, SIGNAL(editTextChanged(const QString&)), this, SLOT(onEditTextChanged(const QString&)));

    setInsertPolicy(QComboBox::NoInsert);
}

void ObjectComboBox::clear()
{
    QComboBox::clear();
    for(int i=0; i < mObjects.size(); i++) {
        mObjects[i]->disconnect(this);
    }
    mObjects.clear();
}

void ObjectComboBox::loadFromScene(Scene * scene)
{
    this->clear();

    if (scene)
        this->setObjects(scene->objects());
}

void ObjectComboBox::loadFromAction(Action * action)
{
    if (! action)
        return;

    this->blockSignals(true);
    this->clear();
    int currIndex = -1, pindex = -1;

    //only load from scene if action is not synced to any resource
    if (!action->resource())
        this->loadFromScene(action->scene());

    Object* parent = action->parentObject();
    if (parent) {
        pindex = mObjects.indexOf(parent);
        if (pindex == -1) {
            insertObject(0, parent);
            pindex = 0;
        }

        if (pindex != -1) {
            this->setItemText(pindex, QString("%1 (%2)").arg(parent->name())
                                                       .arg(tr("parent")));
        }
    }

    if (action->sceneObject()) {
        currIndex = mObjects.indexOf(action->sceneObject());
        if (currIndex == -1) {
            insertObject(pindex+1, action->sceneObject());
            currIndex = pindex + 1;
        }
    }

    this->blockSignals(false);

    if (! action->sceneObject()) {
        if (parent) {
            action->setSceneObject(parent);
            currIndex = pindex;
        }
        else if (!mObjects.isEmpty()) {
            action->setSceneObject(mObjects.first());
            currIndex = 0;
        }
    }

    setCurrentIndex(currIndex);
}

void ObjectComboBox::setObjects(const QList<Object*>& objects)
{
    this->clear();
    for(int i=0; i < objects.size(); i++) {
        if (!objects[i])
            continue;

        addObject(objects[i]);
    }
}

void ObjectComboBox::insertObject(int index, Object* object)
{
    if (! object || (!mTypeFilters.isEmpty() && !mTypeFilters.contains(object->type())))
        return;

    insertItem(index, object->name());
    mObjects.insert(index, object);
    connect(object, SIGNAL(destroyed(Object*)), this, SLOT(objectDestroyed(Object*)), Qt::UniqueConnection);
}

void ObjectComboBox::addObject(Object* object)
{
    insertObject(count(), object);
}

void ObjectComboBox::setCurrentObject(Object * obj)
{
    int index = mObjects.indexOf(obj);
    if (index != -1)
        setCurrentIndex(index);
}

Object* ObjectComboBox::currentObject() const
{
    int currIndex = this->currentIndex();
    if (currIndex >= 0 && currIndex < mObjects.size())
        return mObjects[currIndex];
    return 0;
}

Object* ObjectComboBox::objectAt(int index) const
{
    if (index >= 0 && index < mObjects.size())
        return mObjects.at(index);
    return 0;
}

Object* ObjectComboBox::object(const QString & name) const
{
    foreach(Object* obj, mObjects) {
        if (obj && obj->name() == name)
            return obj;
    }

    return 0;
}

QList<Object*> ObjectComboBox::objects() const
{
    return mObjects;
}

void ObjectComboBox::indexChanged(int index)
{
    if (index >= 0 && index < mObjects.size())
        emit objectChanged(mObjects[index]);
}

void ObjectComboBox::objectDestroyed(Object* object)
{
    removeObject(object);
}

void ObjectComboBox::removeObject(Object * object)
{
    int index = mObjects.indexOf(object);
    if (index != -1) {
        mObjects.removeAt(index);
        if (currentIndex() == index)
            setCurrentIndex(-1);
        removeItem(index);
    }
}

void ObjectComboBox::addTypeFilter(GameObjectMetaType::Type type)
{
    mTypeFilters.append(type);
}

QList<GameObjectMetaType::Type> ObjectComboBox::typeFilters() const
{
    return mTypeFilters;
}

void ObjectComboBox::clearTypeFilters()
{
    mTypeFilters.clear();
}

void ObjectComboBox::onHighlighted(int index)
{
    if (index >= 0 && index < mObjects.size())
        emit objectHighlighted(mObjects.at(index));
}

void ObjectComboBox::onEditTextChanged(const QString & text)
{
    Object* obj = object(text);
    if (obj)
        emit objectChanged(obj);
    else
        emit objectChanged(text);
}

void ObjectComboBox::wheelEvent(QWheelEvent * event)
{
    event->ignore();
}
