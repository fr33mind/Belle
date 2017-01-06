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

#ifndef ACTION_H
#define ACTION_H

#include <QObject>
#include <QIcon>
#include <QString>

#include "gameobject.h"
#include "resource_manager.h"
#include "interaction.h"
#include "action_editor_widget.h"

class ActionEditorWidget;
class Object;
class ResourceManager;
class Wait;

class Action : public GameObject
{
    Q_OBJECT

public:

    explicit Action(QObject *parent = 0);
    Action(const QVariantMap&, QObject *parent = 0);
    virtual ~Action();
    virtual Action* newAction(QObject* parent=0);
    Object* sceneObject() const;
    QString sceneObjectName() const;
    virtual void setSceneObject(Object*);
    void setSceneObject(const QString&);
    void setSceneObjectName(const QString&);
    bool isActive();

    void setTitle(const QString&);
    QString title() const;

    void setDescription(const QString&);
    QString description() const;

    void setAllowSkipping(bool);

    bool supportsEvent(Interaction::InputEvent);
    void setSupportedEvents(Interaction::InputEvents);

    void setDisplayText(const QString&);
    virtual QString displayText() const;

    bool mouseClickOnFinish();
    void setMouseClickOnFinish(bool);

    static void initClass();

    virtual void initFrom(Action*);
    bool allowSkipping() const;
    virtual void paint(const QPainter&);
    virtual QString toString() const;
    virtual QVariantMap toJsonObject(bool internal=true) const;

    virtual void focusIn();
    virtual void focusOut();

    Object* parentObject() const;
    bool isParentTargeted() const;

    bool isTextEditable() const;
    void setTextEditable(bool);

    virtual QString editText() const;
    virtual void setEditText(const QString&);

protected:
    virtual void restoreSceneObject();
    virtual void loadSceneObject();
    virtual void connectSceneObject();
    virtual void disconnectSceneObject();
    virtual void loadData(const QVariantMap&, bool internal=false);
    virtual void connectToResource();

signals:
    void sceneObjectChanged(Object*);

public slots:
    void onSceneObjectDestroyed();

private slots:
    void sceneLoaded();

private:
    Object* mObject;
    QString mObjectName;
    QString mTitle;
    QString mDescription;
    QString mDisplayText;
    bool mAllowSkipping;
    Interaction::InputEvents mSupportedEvents;
    bool mMouseClickOnFinish;
    bool mActive;
    bool mTargetParent;
    bool mTextEditable;

    void init();
    void removeSceneObject();

};

#endif // ACTION_H
