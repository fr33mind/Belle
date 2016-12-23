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

#ifndef GOTOSCENE_H
#define GOTOSCENE_H

#include "action.h"
#include "scene.h"

class Scene;

class GoToScene : public Action
{
    Q_OBJECT

public:
    enum MetaTarget {
        None=0,
        Name,
        Next,
        Previous,
        First,
        Last
    };

public:
    explicit GoToScene(QObject *parent = 0);
    GoToScene(const QVariantMap&, QObject *parent);

    void setMetaTarget(MetaTarget);
    MetaTarget metaTarget() const;

    void setTargetScene(const QString&);
    void setTargetScene(Scene*);
    Scene* targetScene() const;
    QString targetSceneName() const;

    virtual QVariantMap toJsonObject(bool internal=true) const;

    static QString metaTargetString(MetaTarget);
    static MetaTarget metaTargetFromString(const QString&);

    Scene* findScene(const QString&) const;

signals:

public slots:

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);

private slots:
    void updateTargetScene();
    void removeTargetScene();
    void onTargetSceneNameChanged(const QString&);

private:
    void init();
    void updateDisplayText();

    QString mTargetSceneName;
    Scene* mTargetScene;
    MetaTarget mMetaTarget;

};

#endif // GOTOSCENE_H
