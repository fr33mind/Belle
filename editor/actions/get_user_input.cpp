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

#include "get_user_input.h"

#include <QTextCodec>

GetUserInputEditorWidget* mEditorWidget = 0;
ActionInfo GetUserInput::Info;

GetUserInput::GetUserInput(QObject *parent) :
    Action(parent)
{
    init();
}

GetUserInput::GetUserInput(const QVariantMap& data, QObject *parent):
    Action(parent)
{
    init();

    QTextCodec * codec = QTextCodec::codecForName("utf-8");
    if (! codec)
        codec = QTextCodec::codecForLocale();

    if (data.contains("message") && data.value("message").type() == QVariant::String)
        mMessage = codec->toUnicode( data.value("message").toByteArray());

    if (data.contains("variable") && data.value("variable").type() == QVariant::String)
        mVariable = data.value("variable").toString();

    if (data.contains("defaultValue") && data.value("defaultValue").type() == QVariant::String)
        mDefaultValue = codec->toUnicode(data.value("defaultValue").toByteArray());

}

void GetUserInput::init()
{
    setType(Info.type);
    setTypeName(Info.typeName);
    setIcon(Info.icon);
}

void GetUserInput::setGetUserInputEditorWidget(GetUserInputEditorWidget* widget)
{
    mEditorWidget = widget;
}

GetUserInputEditorWidget* GetUserInput::getUserInputEditorWidget()
{
    return mEditorWidget;
}

ActionEditorWidget* GetUserInput::editorWidget()
{
    return mEditorWidget;
}

QString GetUserInput::variable()
{
    return mVariable;
}

void GetUserInput::setVariable(const QString & var)
{
    if (var != mVariable) {
        mVariable = var;
        emit dataChanged();
    }
}

QString GetUserInput::message()
{
    return mMessage;
}

void GetUserInput::setMessage(const QString & msg)
{
    if (msg != mMessage) {
        mMessage = msg;
        emit dataChanged();
    }
}

QString GetUserInput::defaultValue()
{
    return mDefaultValue;
}

void GetUserInput::setDefaultValue(const QString & value)
{
    if (value != mDefaultValue) {
        mDefaultValue = value;
        emit dataChanged();
    }
}

QString GetUserInput::displayText() const
{
    QString var("");
    QString text(tr("Prompt") + " ");

    text += QString("\"%1\"").arg(mMessage);

    if (mVariable.size())
        var = " " + tr("and store reply in") + " $" + mVariable;

    text += var;

    return text;
}

QVariantMap GetUserInput::toJsonObject()
{
    QVariantMap object = Action::toJsonObject();

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    if (! codec)
        codec = QTextCodec::codecForLocale();

    object.insert("message", mMessage);
    object.insert("variable", mVariable);
    object.insert("defaultValue", mDefaultValue);

    return object;
}

