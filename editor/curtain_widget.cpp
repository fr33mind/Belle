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

#include "curtain_widget.h"

CurtainWidget::CurtainWidget(QWidget *parent) :
    QTreeView(parent)
{
    this->setFrameStyle(QFrame::NoFrame);
    mModel = new QStandardItemModel(this);
    QPalette pal = palette();
    pal.setColor(QPalette::Base, pal.color(QPalette::Window));
    this->setPalette(pal);
    this->setModel(mModel);
    this->setHeaderHidden(true);
}

void CurtainWidget::addNewGroup(const QString& title, const QIcon& icon)
{
    QStandardItem *root = new QStandardItem(icon, title);
    mModel->appendRow(root);
}

void CurtainWidget::addWidget(QWidget* widget)
{
    int lastRow = mModel->rowCount()-1;
    if (lastRow < 0)
        return;

    QStandardItem *item = mModel->item(lastRow, 0);
    QStandardItem *child = new QStandardItem;
    item->appendRow(child);
    this->setIndexWidget(child->index(), widget);
    this->expand(child->index().parent());
}
