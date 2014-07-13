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

#include "gotolabel.h"

#include <QDebug>

#include "scene_manager.h"
#include "scene.h"

static GoToLabelEditorWidget* mEditorWidget = 0;
ActionInfo GoToLabel::Info;

GoToLabel::GoToLabel(const QString& name, QObject *parent) :
    Action(parent)
{
    init();
    mTargetLabelName = name;
}

GoToLabel::GoToLabel(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("label") && data.value("label").type() == QVariant::String) {
        setTargetLabel(data.value("label").toString());
    }
}

void GoToLabel::init()
{
    setType(Info.type);
    setName(Info.name);
    setIcon(Info.icon);
    mTargetLabel = 0;
    mTargetLabelName = "";
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease);
}

void GoToLabel::setGoToLabelEditorWidget(GoToLabelEditorWidget * widget)
{
   mEditorWidget = widget;
}

GoToLabelEditorWidget* GoToLabel::goToLabelEditorWidget()
{
   return mEditorWidget;
}

ActionEditorWidget* GoToLabel::editorWidget()
{
    return mEditorWidget;
}

void GoToLabel::setTargetLabel(Label* label)
{
    if (label) {
        mTargetLabel = label;
        mTargetLabelName = label->objectName();
        setDisplayText(mTargetLabelName);
    }
}

void GoToLabel::setTargetLabel(const QString& name)
{
    mTargetLabelName = name;
    setDisplayText(name);
}

bool GoToLabel::isValidLabel(const QString& name)
{
    Scene * scene = this->scene();
    if (! scene)
        return false;

    QList<Action*> actions = scene->actions();
    for(int i=0; i < actions.size(); i++)
        if (actions[i] && qobject_cast<Label*>(actions[i]) && actions[i]->objectName() == name)
            return true;

    return false;
}

bool GoToLabel::hasValidLabel()
{
    return isValidLabel(mTargetLabelName);
}

QString GoToLabel::targetLabelName()
{
    if (mTargetLabel)
        return mTargetLabel->objectName();
    return mTargetLabelName;
}

void GoToLabel::initFrom(Action* action)
{
    Action::initFrom(action);
    GoToLabel* goToLabel = qobject_cast<GoToLabel*>(action);
    mTargetLabelName = goToLabel->targetLabelName();
}

Action* GoToLabel::copy()
{
    GoToLabel* goToLabel = new GoToLabel(mTargetLabelName, parent());
    goToLabel->initFrom(this);
    return goToLabel;
}

QVariantMap GoToLabel::toJsonObject()
{
    QVariantMap action = Action::toJsonObject();
    action.insert("label", mTargetLabelName);

    return action;
}


