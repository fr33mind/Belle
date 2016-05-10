/* Copyright (C) 2012-2014 Carlos Pais
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "scene.h"

#include <QDebug>
#include <QFile>
#include <QtCore/qmath.h>

#include "action.h"
#include "object.h"
#include "scene_manager.h"
#include "textbox.h"
#include "utils.h"
#include "drawing_surface_widget.h"
#include "resource_manager.h"
#include "gameobjectfactory.h"

static QSize mSize;
static QPoint mPoint;
static QPixmap* mScenePixmap = 0;

Scene::Scene(QObject *parent, const QString& name):
    GameObject(parent)
{
    init(name);
}

Scene::Scene(const QVariantMap& data, QObject *parent):
    GameObject(data, parent)
{
    init("");

    if (data.contains("name") && data.value("name").type() == QVariant::String)
        setName(data.value("name").toString());

    if (data.contains("backgroundImage") && data.value("backgroundImage").type() == QVariant::String)
        setBackgroundImage(data.value("backgroundImage").toString());

    if (data.contains("backgroundColor") && data.value("backgroundColor").type() == QVariant::List)
        setBackgroundColor(Utils::listToColor(data.value("backgroundColor").toList()));

    if (data.contains("objects") && data.value("objects").type() == QVariant::List) {
        QVariantList objects = data.value("objects").toList();
        for(int i=0; i < objects.size(); i++) {
            if (objects[i].type() != QVariant::Map)
                continue;
            Object* obj = ResourceManager::instance()->createObject(objects[i].toMap(), this);
            if (obj)
                _appendObject(obj);
        }
    }

    if (data.contains("actions") && data.value("actions").type() == QVariant::List) {
        QVariantList actions = data.value("actions").toList();

        for(int i=0; i < actions.size(); i++) {
            if (actions[i].type() != QVariant::Map)
                continue;

            Action *action = GameObjectFactory::createAction(actions[i].toMap(), this);
            if (action)
                appendAction(action);
        }
    }

    emit loaded();
}


Scene::~Scene()
{
    AssetManager::instance()->releaseAsset(mBackgroundImage);
    if (mScenePixmap)
        delete mScenePixmap;
    mScenePixmap = 0;
    mObjectManager.clear();
    mTemporaryObjectManager.clear();
}

void Scene::init(const QString& name)
{
    if ( parent() )
        connect(parent(), SIGNAL(resized(const QResizeEvent&)), this, SLOT(onResizeEvent(const QResizeEvent&)));

    mTemporaryObjectManager.setUniqueNames(false);
    mTemporaryObjectManager.setAllowEmptyNames(true);
    mSelectedObject = 0;
    mHighlightedObject = 0;
    mBackgroundImage = 0;
    mTemporaryBackgroundImage = 0;
    setType(GameObjectMetaType::Scene);
    //mScenePixmap = new QPixmap(Scene::width(), Scene::height());
    //mScenePixmap->fill(Qt::gray);

    this->setName(name);
}

SceneManager* Scene::sceneManager()
{
    if (this->parent())
        return qobject_cast<SceneManager*>(parent());
    return 0;
}

QList<Object*> Scene::objects() const
{
    QList<Object*> objects;
    for(int i=0; i < mObjectManager.count(); i++)
        objects << qobject_cast<Object*>(mObjectManager.objectAt(i));
    return objects;
}

QList<Object*> Scene::objects(GameObjectMetaType::Type type) const
{
    QList<Object*> objects;
    for(int i=0; i < mObjectManager.count(); i++)
        if (mObjectManager.objectAt(i)->type() == type)
            objects << qobject_cast<Object*>(mObjectManager.objectAt(i));

    return objects;
}

QList<Object*> Scene::temporaryObjects() const
{
    QList<Object*> objects;
    for(int i=0; i < mTemporaryObjectManager.count(); i++)
        objects << qobject_cast<Object*>(mTemporaryObjectManager.objectAt(i));
    return objects;
}

Object* Scene::objectAt (qreal x, qreal y)
{
    QList<Object*> tempObjects = temporaryObjects();
    for(int i=tempObjects.size()-1; i >= 0; --i)
        if (tempObjects[i]->contains(x, y))
            return tempObjects[i]->objectAt(x, y);

    QList<Object*> objects = this->objects();
    for(int i=objects.size()-1; i >= 0; --i)
        if (objects[i]->contains(x, y))
            return objects[i]->objectAt(x, y);

    return 0;
}

Object* Scene::object(const QString& name)
{
    GameObject* obj = mObjectManager.object(name);
    return qobject_cast<Object*>(obj);
}

void Scene::addCopyOfObject(Object* object, bool select)
{
    if (! object)
        return;

    QVariantMap data(object->toJsonObject(true));
    Object * obj = ResourceManager::instance()->createObject(data, this);
    appendObject(obj, select);
    obj->setResource(object);
}

void Scene::_appendObject(Object* object, bool temporary)
{
    if (! object)
        return;

    if (temporary) {
        mTemporaryObjectManager.add(object);
    }
    else {
        if (object->parent() != this)
            object->setParent(this);

        mObjectManager.add(object);
    }

    connect(object, SIGNAL(dataChanged()), DrawingSurfaceWidget::instance(), SLOT(update()));
    connect(object, SIGNAL(destroyed(Object*)), this, SLOT(clearRemovedObject(Object*)));
}

void Scene::_reorderObject(Object* object)
{
    if (object->type() != GameObjectMetaType::TextBox && object->type() != GameObjectMetaType::DialogueBox && object->type() != GameObjectMetaType::Button) {
        int index = mObjectManager.indexOf(object);
        int i=index-1;
        GameObject* obj = mObjectManager.objectAt(i);

        while(i >= 0 && (obj->type() == GameObjectMetaType::TextBox || obj->type() == GameObjectMetaType::DialogueBox || obj->type() == GameObjectMetaType::Button)) {
            --i;
        }

        ++i;
        if (i != index) {
            mObjectManager.takeAt(index);
            mObjectManager.insert(i, object);
        }
    }
}

void Scene::appendObject(Object* object, bool select, bool temporarily)
{
    if (! object)
        return;

    this->_appendObject(object, temporarily);

    if (! temporarily) {
        this->_reorderObject(object);
        emit objectAdded(object);
    }

    if (select)
        selectObject(object);
}

int Scene::width()
{
    return mSize.width();
}

int Scene::height()
{
    return mSize.height();
}

QSize Scene::size()
{
    return mSize;
}

void Scene::setWidth(int w)
{
    mSize.setWidth(w);
}

void Scene::setHeight(int h)
{
    mSize.setHeight(h);
}

void Scene::setSize(const QSize& size)
{
    mSize = size;
}

void Scene::onResizeEvent(const QResizeEvent& event)
{
    emit resized(event);
}

void Scene::selectObjectAt(int x, int y) 
{
    Object *prevSelectedObject = mSelectedObject;
    mSelectedObject = objectAt(x, y);

    //usually returns the self object, but if it is an ObjectGroup it can return other objects inside it
    if (mSelectedObject)
        mSelectedObject = mSelectedObject->objectAt(x, y);

    //if (mSelectedObject != prevSelectedObject)
    selectObject(mSelectedObject);
}

void Scene::moveSelectedObject(int x, int y)
{
    if (mSelectedObject)
        mSelectedObject->dragMove(x, y);
}

void Scene::stopMoveSelectedObject()
{
    if (mSelectedObject) {
        mSelectedObject->stopMove();
        emit dataChanged();
    }
}

Object * Scene::selectedObject()
{
    return mSelectedObject;
}

void Scene::moveSelectedObjectUp()
{
    if (! mSelectedObject)
        return;

    int index = mObjectManager.indexOf(mSelectedObject);
    if (index >= mObjectManager.count()-1)
        return;

    mObjectManager.removeAt(index);
    mObjectManager.insert(index+1, mSelectedObject);
    emit dataChanged();
}

void Scene::moveSelectedObjectDown()
{
    if (! mSelectedObject)
        return;

    int index = mObjectManager.indexOf(mSelectedObject);
    if ( index == 0 )
        return;

    mObjectManager.removeAt(index);
    mObjectManager.insert(index-1, mSelectedObject);
    emit dataChanged();
}

void Scene::fillWidth()
{
    if (! mSelectedObject)
        return;

    mSelectedObject->setX(0);
    mSelectedObject->setWidth(Scene::width());
    emit dataChanged();
}

void Scene::setBackgroundImage(const QString & path)
{
    AssetManager* assetManager = AssetManager::instance();
    ImageFile* image = dynamic_cast<ImageFile*>(assetManager->loadAsset(path, Asset::Image));

    if (mBackgroundImage != image) {
        if (mBackgroundImage && mBackgroundImage->isAnimated()) {
            mBackgroundImage->movie()->stop();
            mBackgroundImage->movie()->disconnect(this);
        }
        assetManager->releaseAsset(mBackgroundImage);

        if (image && image->isValid()) {
            if (image->isAnimated()) {
                connect(image->movie(), SIGNAL(frameChanged(int)), this, SIGNAL(dataChanged()));
                image->movie()->setScaledSize(Scene::size());
                image->movie()->start();
            }
        }

        mBackgroundImage = image;
        emit dataChanged();
    }
}

ImageFile* Scene::backgroundImage()
{
    return mBackgroundImage;
}

void Scene::setTemporaryBackgroundImage(ImageFile* image)
{
    if (mTemporaryBackgroundImage != image) {
        if (mTemporaryBackgroundImage && mTemporaryBackgroundImage->isAnimated()) { //disconnect previous background
            AnimatedImage* anim = dynamic_cast<AnimatedImage*>(mTemporaryBackgroundImage);
            anim->movie()->stop();
            anim->movie()->disconnect(this);
        }

        if (image && image->isAnimated()) {
            AnimatedImage* anim = dynamic_cast<AnimatedImage*>(image);
            connect(anim->movie(), SIGNAL(frameChanged(int)), this, SIGNAL(dataChanged()));
            image->movie()->setScaledSize(Scene::size());
            anim->movie()->start();
        }

        mTemporaryBackgroundImage = image;
        emit dataChanged();
    }
}

ImageFile* Scene::temporaryBackgroundImage()
{
    return mTemporaryBackgroundImage;
}

void Scene::setBackgroundColor(const QColor& color)
{
    if (mBackgroundColor != color) {
        mBackgroundColor = color;
        emit dataChanged();
    }
}

QColor Scene::backgroundColor()
{
    return mBackgroundColor;
}

void Scene::setTemporaryBackgroundColor(const QColor& color)
{
    if (mTemporaryBackgroundColor != color) {
        mTemporaryBackgroundColor = color;
        emit dataChanged();
    }
}

QColor Scene::temporaryBackgroundColor()
{
    return mTemporaryBackgroundColor;
}

QString Scene::backgroundPath()
{
    if (mBackgroundImage)
        return mBackgroundImage->path();
    return "";
}

ImageFile* Scene::background() const
{
    return mBackgroundImage;
}

void Scene::clearBackground()
{
    if ( mBackgroundImage ) {
        AssetManager::instance()->releaseAsset(mBackgroundImage);
        mBackgroundImage = 0;
        emit dataChanged();
    }
}

QPoint Scene::point()
{
    return mPoint;
}

void Scene::setPoint(const QPoint & point)
{
    mPoint = point;
}

void Scene::removeObject(Object* object, bool del)
{
    if (! object)
        return;

    bool removed = false;

    if (mTemporaryObjectManager.contains(object))
        removed = mTemporaryObjectManager.remove(object);
    else if (mObjectManager.contains(object))
        removed = mObjectManager.remove(object);

    if (removed) {
        clearRemovedObject(object);
        if (del && object->parent() == this) {
            object->deleteLater();
        }
    }
}

void Scene::clearRemovedObject(Object *object)
{
    emit objectRemoved(object);
    if (selectedObject() == object)
        selectObject(0);
    object->disconnect(this);
    emit dataChanged();
}

void Scene::removeSelectedObject(bool del)
{
    removeObject(mSelectedObject, del);
    mSelectedObject = 0;
}

void Scene::selectObject(Object* obj)
{

    mHighlightedObject = 0;
    mSelectedObject = obj;

   emit dataChanged();
   emit selectionChanged(mSelectedObject);
}

void Scene::highlightObject(Object* obj)
{
    if (mObjectManager.contains(obj) || ! obj) {
        mHighlightedObject = obj;
        emit dataChanged();
    }
}

Object* Scene::highlightedObject()
{
    return mHighlightedObject;
}

void Scene::removeActionAt(int index , bool del)
{
    if (index >= 0 && index < mActions.size()) {
        Action * action = mActions.takeAt(index);
        emit actionRemoved(action);
        emit actionRemoved(index);

        if (del)
            action->deleteLater();
    }
}

void Scene::removeAction(Action* action, bool del)
{
    if (! action)
        return;
    removeActionAt(mActions.indexOf(action), del);
}

QList<Action*> Scene::actions() const
{
    return mActions;
}

void Scene::setActions(const QList<Action *> & actions)
{
    mActions = actions;
}

void Scene::appendAction(Action * action, bool copy)
{
    insertAction(mActions.size(), action, copy);
}

void Scene::insertAction(int row, Action* action, bool copy)
{
    if (! action)
        return;

    if (copy)
        action = GameObjectFactory::createAction(action->toJsonObject(), this);
    else
        action->setParent(this);
    mActions.insert(row, action);
    emit actionInserted(row, action);
}

Action* Scene::actionAt(int i) const
{
    if (i >= 0 && i < mActions.size())
        return mActions.at(i);
    return 0;
}

QIcon Scene::icon()
{
    if (! mScenePixmap)
        mScenePixmap = new QPixmap(Scene::width(), Scene::height());

    if (mScenePixmap) {
        //update pixmap
        QPainter painter(mScenePixmap);
        this->paint(painter);
        painter.end();
        QPixmap pixmap = mScenePixmap->scaled(64, 48, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        return QIcon(pixmap);
    }

    return QIcon();
}

QPixmap* Scene::pixmap()
{
    return mScenePixmap;
}

QVariantMap Scene::toJsonObject(bool internal)
{
    QVariantMap scene = GameObject::toJsonObject(internal);

    if (mBackgroundImage)
        scene.insert("backgroundImage", mBackgroundImage->name());

    if (mBackgroundColor.isValid())
        scene.insert("backgroundColor", Utils::colorToList(mBackgroundColor));

    QVariantList objects;
    for(int i=0; i < mObjectManager.count(); i++) {
        objects.append(mObjectManager.objectAt(i)->toJsonObject(internal));
    }
    scene.insert("objects", objects);

    QVariantList actions;
    for(int i=0; i < mActions.size(); i++) {
        actions.append(mActions[i]->toJsonObject(internal));
    }

    scene.insert("actions", actions);

    return scene;
}

Scene* Scene::copy()
{
    Scene* scene = new Scene(this->toJsonObject(), this->parent());
    scene->setName(name());
    return scene;
}

void Scene::show()
{
    if (mBackgroundImage && mBackgroundImage->isAnimated()) {
        AnimatedImage* anim = dynamic_cast<AnimatedImage*>(mBackgroundImage);
        anim->movie()->start();
    }

    QList<Object*> objects = this->objects();
    for(int i=0; i < objects.size(); i++)
        objects[i]->show();
}

void Scene::hide()
{
    removeTemporaryBackground();
    if (mBackgroundImage && mBackgroundImage->isAnimated()) {
        AnimatedImage* anim = dynamic_cast<AnimatedImage*>(mBackgroundImage);
        anim->movie()->stop();
    }

    QList<Object*> objects = this->objects();
    for(int i=0; i < objects.size(); i++)
        objects[i]->hide();
}

void Scene::removeTemporaryBackground()
{
    if (mTemporaryBackgroundImage) {
        if (mTemporaryBackgroundImage->isAnimated()) {
            AnimatedImage* anim = dynamic_cast<AnimatedImage*>(mBackgroundImage);
            anim->movie()->stop();
            anim->movie()->disconnect(this);
        }
        mTemporaryBackgroundImage = 0;
    }

    if (mTemporaryBackgroundColor.isValid())
        mTemporaryBackgroundColor = QColor();
}

void Scene::paint(QPainter & painter)
{
    QColor bgColor = backgroundColor().isValid() ? backgroundColor() : Qt::gray;

    if (mTemporaryBackgroundImage && mTemporaryBackgroundImage->isValid()) {
        painter.drawPixmap(0, 0, Scene::width(), Scene::height(), mTemporaryBackgroundImage->pixmap());
    }
    else if (mTemporaryBackgroundColor.isValid())
        painter.fillRect(QRect(Scene::point().x(), Scene::point().y(), width(), height()), mTemporaryBackgroundColor);
    else if (mBackgroundImage && mBackgroundImage->isValid()) {
        painter.drawPixmap(0, 0, Scene::width(), Scene::height(), mBackgroundImage->pixmap());
    }
    else
        painter.fillRect(QRect(Scene::point().x(), Scene::point().y(), width(), height()), bgColor);

    QFont font;
    QPen defaultPen;
    painter.setPen(defaultPen);
    //font.setFamily((QFontDatabase::applicationFontFamilies(0).at(0).toLocal8Bit().constData()));
    font.setPointSize(20);
    painter.setFont(font);

    QList<Object*> objects = this->objects();
    Object * object;

    for (int i=0; i < objects.size(); i++) {
        object = objects.at(i);
        if (object){
            painter.save();
            object->paint(painter);
            painter.restore();
        }
    }

    objects = this->temporaryObjects();
    for (int i=0; i < objects.size(); i++) {
        object = objects.at(i);
        if (object && object->visible()){
            painter.save();
            object->paint(painter);
            painter.restore();
        }
    }

    if(this->highlightedObject()) {
        QRectF rectf = this->highlightedObject()->sceneRect();
        painter.save();
        QBrush brush(QColor(0,0, 255, 100));
        QPen pen(brush, 6);
        painter.setPen(pen);
        painter.drawRect(rectf);
        painter.restore();
    }
}

void Scene::resize(int w, int h, bool pos, bool size)
{
    qreal wratio = w / (Scene::width() * 1.0);
    qreal hratio = h / (Scene::height() * 1.0);

    QList<Object*> objects = this->objects();
    for(int i=0; i < objects.size(); i++) {
        Object * obj = objects[i];
        if (pos) {
            obj->setX(qRound(obj->x()*wratio));
            obj->setY(qRound(obj->y()*hratio));
        }
        if (size) {
            obj->setWidth(qRound(obj->width()*wratio));
            obj->setHeight(qRound(obj->height()*hratio));
        }
    }
}

int Scene::indexOf(GameObject* obj)
{
    if (qobject_cast<Object*>(obj)) {
        return mObjectManager.indexOf(obj);
    }
    else if (qobject_cast<Action*>(obj)) {
        return mActions.indexOf(qobject_cast<Action*>(obj));
    }

    return -1;
}
