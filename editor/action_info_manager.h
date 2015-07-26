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

#ifndef ACTION_INFO_MANAGER_H
#define ACTION_INFO_MANAGER_H

#include <QObject>
#include <QFlags>
#include <QEvent>
#include <QIcon>

#include "interaction.h"

class ActionEditorWidget;
class Action;


/*class Action
{

public:
    enum InputEvent{
        None = 0,
        MousePress = 1,
        MouseRelease = 2,
        MouseEnter = 4,
        MouseLeave = 8
    };

    Q_DECLARE_FLAGS(Interaction::InputEvents, InputEvent)
    Action(const QIcon& icon, const QString& name, const QString& description, Interaction::InputEvents Events, ActionEditorWidget* editor, int index)
    {
        mIcon = icon;
        mName = name;
        mDescription = description;
        mEditor = editor;
        mIndex = index;
        mSupportedEvents = Events;
    }

    QIcon icon() {return mIcon;}
    void setIcon(const QIcon& icon) { mIcon = icon; }

    QString name() {return mName;}
    void setName(const QString& name) {mName = name;}

    ActionEditorWidget* editor() { return mEditor; }

    bool supportsEvent(InputEvent event) { return mSupportedEvents.testFlag(event); }

private:
    QIcon mIcon;
    QString mName;
    QString mDescription;
    int mIndex;
    ActionEditorWidget* mEditor;
    Interaction::InputEvents mSupportedEvents;

};*/

class ActionInfoManager : public QObject
{
    Q_OBJECT

public:
    explicit ActionInfoManager(QObject *parent = 0);
    static void init();
    static void registerAction(const QIcon&, const QString&, const QString&, Interaction::InputEvents events, Action*);
    static void registerActions();
    static Action* typeToAction(const QVariant&, QObject*);
    static QList<Action*> actions();
    static void destroy();

signals:

public slots:

};



#endif // ACTION_MANAGER_H
