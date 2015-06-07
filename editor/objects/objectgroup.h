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

#ifndef OBJECTGROUP_H
#define OBJECTGROUP_H

#include "object.h"
#include "objectgroup_editor_widget.h"

class ObjectGroupEditorWidget;

class ObjectGroup : public Object
{
    Q_OBJECT

    Object* mSelectedObject;

public:
    explicit ObjectGroup(QObject *parent = 0, const QString& name="ObjectGroup");
    ObjectGroup(const QVariantMap& data, QObject *parent = 0);

    static ObjectGroupEditorWidget* objectGroupEditorWidget();
    static void setObjectGroupEditorWidget(ObjectGroupEditorWidget*);
    virtual ObjectEditorWidget* editorWidget();

    void append(Object*, int spacing=0);
    Object* object(int);
    Object* object(const QString&);
    Object* object(const QPoint&);
    QList<Object*> objects() const;
    QVariantList variantObjects() const;
    virtual void paint(QPainter&);
    virtual void move(int x, int y);
    virtual QVariantMap toJsonObject(bool internal=true);
    virtual Object* objectAt(qreal, qreal);
    void adaptSize();
    virtual void resize(int, int);
    void removeObjectAt(int, bool del=false);
    void removeAllObjects(bool del=false);
    int calcSpacing() const;
    int minHeight() const;

    void setWidth(int, bool percent=false);
    void setHeight(int, bool percent=false);
    void setX(int);
    void setY(int);

    bool editingMode();
    void setEditingMode(bool);

signals:

public slots:
    virtual void load(const QVariantMap &);

private slots:
    void objectDestroyed(Object*);
    void objectChanged(const QVariantMap&);

private:
    QList<Object*> mObjects;
    QList<Object*> mStickyObjects;
    bool mEditingMode;
    void init();
    void _load(const QVariantMap&);
    int indexOf(Object*);
    void alignObjects();
    void alignObjectsHorizontally();
    void alignObjectsVertically();
    void checkStickyObjects();
    void addStickyObject(Object*);
    void _append(Object*);
};

#endif // OBJECTGROUP_H
