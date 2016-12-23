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
#include "scene_manager.h"
#include "scene.h"
#include "belle.h"

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
    loadInternal(data);
    connect(Belle::instance(), SIGNAL(projectLoaded()), this, SLOT(updateTargetLabel()), Qt::UniqueConnection);
}

void GoToLabel::init()
{
    setType(GameObjectMetaType::GoToLabel);
    mTargetLabel = 0;
    mTargetLabelName = "";
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease);
}

void GoToLabel::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    if (data.contains("label") && data.value("label").type() == QVariant::String) {
        setTargetLabel(data.value("label").toString());
    }
}

void GoToLabel::setTargetLabel(Label* label)
{
    if (mTargetLabel == label)
        return;

    removeTargetLabel();
    mTargetLabel = label;
    mTargetLabelName = "";

    if (mTargetLabel) {
        mTargetLabelName = mTargetLabel->name();
        connect(mTargetLabel, SIGNAL(nameChanged(const QString&)), this, SLOT(onLabelNameChanged(const QString&)));
    }

    setDisplayText(mTargetLabelName);
    notify("label", mTargetLabelName);
}

void GoToLabel::setTargetLabel(const QString& name)
{
    if (!mTargetLabel || mTargetLabel->name() != name) {
        Label* label = findLabel(name);
        setTargetLabel(label);
    }

    if (mTargetLabelName == name)
        return;

    if (!mTargetLabel) {
        mTargetLabelName = name;
        setDisplayText(mTargetLabelName);
        notify("label", mTargetLabelName);
    }
}

bool GoToLabel::isValidLabel(const QString& name)
{
    if (findLabel(name))
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

QVariantMap GoToLabel::toJsonObject(bool internal) const
{
    QVariantMap action = Action::toJsonObject(internal);
    action.insert("label", mTargetLabelName);

    return action;
}

QList<Label*> GoToLabel::availableLabels() const
{
    Scene * scene = this->scene();
    QList<Label*> labels;
    if (! scene)
        return labels;

    QList<Action*> actions = scene->actions();
    for(int i=0; i < actions.size(); i++) {
        if (actions[i] && actions[i]->type() == GameObjectMetaType::Label)
            labels.append(qobject_cast<Label*>(actions[i]));
    }

    return labels;
}

void GoToLabel::removeTargetLabel()
{
    if (!mTargetLabel)
        return;

    mTargetLabel->disconnect(this);
    mTargetLabel = 0;
    mTargetLabelName = "";
    setDisplayText("");
}

Label* GoToLabel::findLabel(const QString& name) const
{
    QList<Label*> labels = availableLabels();

    for(int i=0; i < labels.size(); i++)
        if (labels[i]->name() == name)
            return labels[i];

    return 0;
}

void GoToLabel::onLabelNameChanged(const QString& name)
{
    mTargetLabelName = name;
    setDisplayText(mTargetLabelName);
}

void GoToLabel::updateTargetLabel()
{
    setTargetLabel(mTargetLabelName);
    Belle::instance()->disconnect(this);
}
