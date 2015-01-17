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

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#if defined(Q_WS_X11)
    #define RESOURCES_DEFAULT_PATH "/usr/share/belle/resources"
#elif defined(Q_WS_MAC)
    #define RESOURCES_DEFAULT_PATH "Belle.app/Contents/Resources/engine"
#else
    #define RESOURCES_DEFAULT_PATH  "resources"
#endif

#include <QObject>
#include <QList>
#include <QDir>

#include "objects/object.h"
#include "imagefile.h"

class Object;

class ResourceManager : public QObject
{
    Q_OBJECT

public:
    explicit ResourceManager(QObject *parent = 0);
    void addResource(Object*);
    Object* createResource(QVariantMap, bool appendToResources=true, QObject* parent=0);
    bool contains(const QString&);
    void removeResource(Object*, bool del=false);
    static Object* resource(const QString&);
    static Object* resource(int);
    static void fillWithResourceData(QVariantMap&);
    Object* typeToObject(const QString&, QVariantMap& extraData, QObject* parent=0);
    bool isValidName(const QString&);
    QString newName(QString);
    QString newName(Object*);
    static void setRelativePath(const QString&);
    static ResourceManager* instance();
    static QList<Object*> resources();
    static void importResources(const QVariantMap&);
    static QVariantMap exportResources();

    static void destroy();
    void removeResources(bool del);

signals:
    void resourceAdded(Object*);
    void resourceRemoved(Object*);
    void resourceRemoved(int);
    void resourceChanged();

public slots:


};

#endif // RESOURCE_MANAGER_H
