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

#include <QDebug>

#include "scene_manager.h"

static GoToSceneEditorWidget* mEditorWidget = 0;
ActionInfo GoToScene::Info;

GoToScene::GoToScene(QObject *parent) :
    Action(parent)
{
    init();
}

GoToScene::GoToScene(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("target") && data.value("target").type() == QVariant::String) {
        mTargetScene = data.value("target").toString();
        setDisplayText(mTargetScene);
    }

    if (data.contains("targetType") && data.value("targetType").canConvert(QVariant::Int)) {
        mTargetType = static_cast<GoToScene::TargetType>(data.value("targetType").toInt());
    }
}

void GoToScene::init()
{
    setType(Info.type);
    setTypeName(Info.typeName);
    setIcon(Info.icon);
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease);
    mTargetScene = "";
    mTargetType = Name;
}

void GoToScene::setGoToSceneEditorWidget(GoToSceneEditorWidget * widget)
{
   mEditorWidget = widget;
}

GoToSceneEditorWidget* GoToScene::goToSceneEditorWidget()
{
   return mEditorWidget;
}

ActionEditorWidget* GoToScene::editorWidget()
{
    return mEditorWidget;
}

void GoToScene::setTargetScene(const QString & name, TargetType type)
{
    mTargetScene = name;
    mTargetType = type;
    setDisplayText(mTargetScene);
}

QString GoToScene::targetScene()
{
    return mTargetScene;
}

QVariantMap GoToScene::toJsonObject()
{
    QVariantMap action = Action::toJsonObject();
    if (! mTargetScene.isEmpty()) {
        action.insert("target", mTargetScene);
        action.insert("targetType", mTargetType);
    }
    return action;
}
