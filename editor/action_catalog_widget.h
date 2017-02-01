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

#ifndef ACTION_CATALOG_VIEW_H
#define ACTION_CATALOG_VIEW_H

#include "properties_widget.h"
#include "action.h"
#include "gameobjectmetatype.h"

class ActionCatalogWidget : public PropertiesWidget
{
    Q_OBJECT

public:
    explicit ActionCatalogWidget(QWidget *parent = 0);

    void hideAction(GameObjectMetaType::Type);
    void hideActions(const QList<GameObjectMetaType::Type>&);

    void showAction(GameObjectMetaType::Type);
    void showActions(const QList<GameObjectMetaType::Type>&);

signals:
    void newAction(Action*);

public slots:
    void onDoubleClick(const QModelIndex&);

private:
    void _appendRow(const GameObjectMetaType*);

};

#endif // ACTION_CATALOG_VIEW_H
