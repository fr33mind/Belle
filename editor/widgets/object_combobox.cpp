#include "object_combobox.h"

ObjectComboBox::ObjectComboBox(QWidget *parent) :
    QComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
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

    this->loadFromScene(action->scene());

    Object* parent = qobject_cast<Object*>(action->parent());
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
        addObject(objects[i]);
    }
}

void ObjectComboBox::insertObject(int index, Object* object)
{
    if (! object)
        return;

    insertItem(index, object->name());
    mObjects.insert(index, object);
    connect(object, SIGNAL(destroyed(Object*)), this, SLOT(objectDestroyed(Object*)), Qt::UniqueConnection);
}

void ObjectComboBox::addObject(Object* object)
{
    insertObject(count(), object);
}


Object* ObjectComboBox::currentObject() const
{
    int currIndex = this->currentIndex();
    if (currIndex >= 0 && currIndex < mObjects.size())
        return mObjects[currIndex];
    return 0;
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
