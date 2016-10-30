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

#ifndef CHANGE_BACKGROUND_H
#define CHANGE_BACKGROUND_H

#include "action.h"

class ChangeBackgroundEditorWidget;

class ChangeBackground : public Action
{
    Q_OBJECT

    ImageFile* mBackgroundImage;
    QColor mBackgroundColor;

public:
    explicit ChangeBackground(QObject *parent = 0);
    ChangeBackground(const QVariantMap&, QObject *parent=0);
    virtual ~ChangeBackground();
    virtual QVariantMap toJsonObject(bool internal=true) const;

    void setBackgroundImage(const QString&);
    QString backgroundPath();
    ImageFile* background() const;

    void setBackgroundColor(const QColor&);
    QColor backgroundColor();

    void focusIn();
    void focusOut();
    
signals:
    
public slots:

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);

private:
   void init();
    
};

#endif // CHANGE_BACKGROUND_H
