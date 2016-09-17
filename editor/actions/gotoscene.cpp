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

#include "gotoscene.h"
#include "scene_manager.h"
#include "belle.h"

GoToScene::GoToScene(QObject *parent) :
    Action(parent)
{
    init();
}

GoToScene::GoToScene(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    connect(Belle::instance(), SIGNAL(projectLoaded()), this, SLOT(updateTargetScene()), Qt::UniqueConnection);

    //targetType is now deprecated. Remove at some point.
    if (data.contains("targetType") && data.value("targetType").canConvert(QVariant::Int)) {
        QString target = data.value("target").toString();
        if (data.value("targetType").toInt() == 1)
            mMetaTarget = metaTargetFromString(target);
        else
            setTargetScene(target);
    }
    else {
        if (data.contains("metaTarget") && data.value("metaTarget").canConvert(QVariant::Int)) {
            setMetaTarget(static_cast<GoToScene::MetaTarget>(data.value("metaTarget").toInt()));
        }

        if (data.contains("target") && data.value("target").type() == QVariant::String) {
            setTargetScene(data.value("target").toString());
        }
    }
}

void GoToScene::init()
{
    setType(GameObjectMetaType::GoToScene);
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease);
    mTargetSceneName = "";
    mTargetScene = 0;
    mMetaTarget = None;
}

void GoToScene::updateTargetScene()
{
    setTargetScene(mTargetSceneName);
    Belle::instance()->disconnect(this);
}

void GoToScene::removeTargetScene()
{
    if (!mTargetScene)
        return;

    mTargetScene->disconnect(this);
    mTargetScene = 0;
    mTargetSceneName = "";
    setDisplayText("");
}

void GoToScene::setMetaTarget(MetaTarget target)
{
    if (mMetaTarget == target)
        return;

    mMetaTarget = target;
    if (mMetaTarget != GoToScene::Name)
        removeTargetScene();

    updateDisplayText();
}

GoToScene::MetaTarget GoToScene::metaTarget() const
{
    return mMetaTarget;
}

QString GoToScene::targetSceneName() const
{
    if (mTargetScene)
        return mTargetScene->name();
    return mTargetSceneName;
}

void GoToScene::setTargetScene(const QString & name)
{
    if (mTargetSceneName != name) {
        mTargetSceneName = name;
        setMetaTarget(GoToScene::Name);
        updateDisplayText();
    }

    Scene* scene = this->scene();
    if (!scene)
        return;
    SceneManager* sceneManager = scene->sceneManager();
    if (!sceneManager)
        return;
    Scene* targetScene = sceneManager->scene(name);
    if (targetScene)
        setTargetScene(targetScene);
}

void GoToScene::setTargetScene(Scene* scene)
{
    if (scene == targetScene())
        return;

    removeTargetScene();

    mTargetScene = scene;
    mTargetSceneName = "";
    setMetaTarget(GoToScene::Name);

    if (mTargetScene) {
        connect(mTargetScene, SIGNAL(destroyed()), this, SLOT(removeTargetScene()));
        connect(mTargetScene, SIGNAL(nameChanged(const QString&)), this, SLOT(onTargetSceneNameChanged(const QString&)));
    }
    else {
        setMetaTarget(GoToScene::None);
    }

    updateDisplayText();
}

Scene* GoToScene::targetScene() const
{
    return mTargetScene;
}

void GoToScene::onTargetSceneNameChanged(const QString & name)
{
    updateDisplayText();
}

QString GoToScene::metaTargetString(MetaTarget target)
{
    switch(target)
    {
        case None: return "None";
        case Name: return "Scene";
        case Next: return "Next";
        case Previous: return "Previous";
        case First: return "First";
        case Last: return "Last";
        default: break;
    }

    return "";
}

GoToScene::MetaTarget GoToScene::metaTargetFromString(const QString& name)
{
    if (name == "Next")
        return GoToScene::Next;
    else if (name == "Previous")
        return GoToScene::Previous;
    else if (name == "First")
        return GoToScene::First;
    else if (name == "Last")
        return GoToScene::Last;

    return GoToScene::Name;
}

void GoToScene::updateDisplayText()
{
    if (mMetaTarget == GoToScene::Name)
        setDisplayText(QString("\"%1\"").arg(targetSceneName()));
    else
        setDisplayText(metaTargetString(mMetaTarget));
}

QVariantMap GoToScene::toJsonObject(bool internal) const
{
    QVariantMap action = Action::toJsonObject(internal);
    QString sceneName = targetSceneName();
    if (!sceneName.isEmpty())
        action.insert("target", sceneName);
    action.insert("metaTarget", static_cast<int>(mMetaTarget));
    return action;
}
