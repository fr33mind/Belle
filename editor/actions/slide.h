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
#include "slide_editor_widget.h"

class SlideEditorWidget;

class Slide : public Action
{
    Q_OBJECT

    QPoint mStartPoint;
    QPoint mEndPoint;
    QPoint mObjectOriginalPoint;
    float mDuration;

public:
    static ActionInfo Info;

public:
    explicit Slide(QObject *parent = 0);
    Slide(const QVariantMap&, QObject* parent = 0);
    static SlideEditorWidget* slideEditorWidget();
    static void setSlideEditorWidget(SlideEditorWidget*);
    virtual ActionEditorWidget* editorWidget();
    virtual QString displayText() const;
    int startX() const;
    int startY() const;
    int endX() const;
    int endY() const;
    QPoint startPoint() const;
    QPoint endPoint() const;
    void setStartPoint(const QPoint&);
    void setStartX(int);
    void setStartY(int);
    void setEndPoint(const QPoint&);
    void setEndX(int);
    void setEndY(int);

    float duration() const;
    void setDuration(float);
    //void setSceneObject(Object *);
    virtual void finishedEditing();
    virtual QVariantMap toJsonObject();

signals:
    void startPositionChanged(int, int);

private slots:
    void objectPositionChanged(int, int);

private:
    void init();


};

#endif // SLIDE_H
