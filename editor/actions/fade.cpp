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

#include "fade.h"

#include <QDebug>

static FadeEditorWidget * mEditorWidget = 0;
ActionInfo Fade::Info;

Fade::Fade(QObject *parent) :
    Action(parent)
{
    init();
}

Fade::Fade(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("fadeType") && data.value("fadeType").type() == QVariant::String)
        setFadeType(data.value("fadeType").toString());

    if (data.contains("duration") && data.value("duration").canConvert(QVariant::Double))
        mDuration = data.value("duration").toDouble();
}

void Fade::init()
{
    setType(Info.type);
    setTypeName(Info.typeName);
    setIcon(Info.icon);
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease |
    Interaction::MouseMove);
    setFadeType(Fade::Out);
    mDuration = 1;
}

void Fade::setFadeEditorWidget(FadeEditorWidget * widget)
{
   mEditorWidget = widget;
}

FadeEditorWidget* Fade::fadeEditorWidget()
{
   return mEditorWidget;
}

ActionEditorWidget* Fade::editorWidget()
{
    return mEditorWidget;
}

Fade::Type Fade::fadeType()
{
    return mFadeType;
}

QString Fade::fadeTypeString() const
{
    return fadeTypeToString(mFadeType);
}

void Fade::setFadeType(Fade::Type type)
{
    mFadeType = type;
    QString namepart(tr("out"));

    if (type == Fade::In)
        namepart = tr("in");

    setTypeName(QString("%1 %2").arg(tr("Fade")).arg(namepart));
    emit dataChanged();
}

void Fade::setFadeType(const QString& type)
{
    if (type == "in")
        setFadeType(Fade::In);
    else
        setFadeType(Fade::Out);
}

QString Fade::fadeTypeToString(Fade::Type type) const
{
    if (type == 0)
        return "in";

    return "out";
}

double Fade::duration() const
{
    return mDuration;
}

void Fade::setDuration(double duration)
{
    if (duration != mDuration) {
        mDuration = duration;
        emit dataChanged();
    }
}

QString Fade::displayText() const
{
    if (! this->sceneObject())
        return "";
    return QString("\"%1\" %2 %3s").arg(sceneObject()->objectName()).arg(tr("in")).arg(duration());
}

QVariantMap Fade::toJsonObject()
{
    QVariantMap data = Action::toJsonObject();
    data.insert("fadeType", fadeTypeToString(mFadeType));
    data.insert("duration", QVariant(mDuration));

    return data;
}
