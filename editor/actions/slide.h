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

#ifndef SLIDE_H
#define SLIDE_H

#include <QPoint>

#include "action.h"

class Slide : public Action
{
    Q_OBJECT

public:
    static ActionInfo Info;

    enum Type {
        In,
        Out,
        Custom
    };

public:
    explicit Slide(QObject *parent = 0);
    Slide(const QVariantMap&, QObject* parent = 0);
    virtual QString displayText() const;
    Type slideType() const;
    QString slideTypeAsString() const;
    void setSlideType(Type);
    void setSlideType(const QString&);
    void setDestX(int);
    void setDestX(const QString&);
    QString destX() const;
    void setDestY(int);
    void setDestY(const QString&);
    QString destY() const;
    double duration() const;
    void setDuration(double);
    virtual void finishedEditing();
    virtual QVariantMap toJsonObject();

signals:
    void startPositionChanged(int, int);

private slots:
    void objectPositionChanged(int, int);

private:
    Type mSlideType;
    QString mDestX;
    QString mDestY;
    QPoint mObjectOriginalPoint;
    double mDuration;

    void init();
    void updateSlideType();
};

#endif // SLIDE_H
