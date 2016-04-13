#ifndef GAMEOBJECTFACTORY_H
#define GAMEOBJECTFACTORY_H

#include <QObject>
#include <QVariant>
#include "gameobject.h"
#include "gameobjectmetatype.h"
#include "action.h"
#include "object.h"
#include "scene.h"

class GameObject;
class Action;
class Object;
class Scene;

class GameObjectFactory : public QObject
{
    Q_OBJECT

public:
    static void init();
    static void destroy();
    static GameObject* createGameObject(GameObjectMetaType::Type, const QVariantMap&, QObject* parent=0);
    static GameObject* createGameObject(GameObjectMetaType::Type, QObject* parent=0);
    static GameObject* createGameObject(const QVariantMap&, QObject* parent=0);
    static Action* createAction(GameObjectMetaType::Type, const QVariantMap& data, QObject* parent=0);
    static Action* createAction(GameObjectMetaType::Type, QObject* parent=0);
    static Action* createAction(const QVariantMap&, QObject* parent=0);
    static Object* createObject(GameObjectMetaType::Type, const QVariantMap&, QObject* parent=0);
    static Object* createObject(GameObjectMetaType::Type, QObject* parent=0);
    static Object* createObject(const QVariantMap&, QObject* parent=0);
    static Scene* createScene(const QVariantMap&, QObject* parent=0);

signals:

public slots:

};

#endif // GAMEOBJECTFACTORY_H
