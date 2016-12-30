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

class ObjectGroup : public Object
{
    Q_OBJECT

    Object* mSelectedObject;
    int mSpacing;

public:
    explicit ObjectGroup(QObject *parent = 0, const QString& name="");
    ObjectGroup(const QVariantMap& data, QObject *parent = 0);
    void append(Object*);
    Object* object(int) const;
    Object* object(const QString&) const;
    Object* object(const QPoint&) const;
    QList<Object*> objects() const;
    QVariantList variantObjects() const;
    virtual void paint(QPainter&);
    virtual void move(int x, int y);
    virtual QVariantMap toJsonObject(bool internal=true) const;
    virtual Object* objectAt(qreal, qreal);
    void adaptSize();
    virtual void resize(int, int);
    void removeObjectAt(int, bool del=false);
    void removeAllObjects(bool del=false);
    int minHeight() const;
    void setSpacing(int);
    int spacing() const;

    bool isAlignEnabled() const;
    void setAlignEnabled(bool);

    void setWidth(int, bool percent=false);
    void setHeight(int, bool percent=false);
    void setX(int);
    void setY(int);

    bool editingMode() const;
    void setEditingMode(bool);

signals:

private slots:
    void objectDestroyed(Object*);
    void objectChanged(const QVariantMap&);

protected:
    void _append(Object*);
    QVariantMap prepareObjectData(int, const QVariantMap&);
    virtual void loadData(const QVariantMap&, bool internal=false);
    Object* createObject(const QVariantMap&);
    void checkStickyObjects();
    int calcSpacing() const;
    void updateSpacing();
    void adaptLayout();
    virtual void loadObject(Object*, const QVariantMap&);

private:
    QList<Object*> mObjects;
    QList<Object*> mStickyObjects;
    bool mEditingMode;
    bool mAligning;
    bool mAlignEnabled;
    void init();
    int indexOf(Object*);
    void alignObjects();
    void alignObjectsHorizontally();
    void alignObjectsVertically();
    void addStickyObject(Object*);
    QVariantList objectsRelativeRectsData();
};

#endif // OBJECTGROUP_H
