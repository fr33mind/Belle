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

#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <QObject>
#include <QPixmap>
#include <QRectF>
#include <QResizeEvent>
#include <QString>
#include <QHash>

#include "gameobject.h"
#include "action.h"
#include "interaction.h"
#include "padding.h"
#include "animatedimage.h"
#include "background.h"
#include "assetmanager.h"

#define RESIZE_RECT_WIDTH 7

class Scene;
class Action;

class Object : public GameObject
{
    Q_OBJECT
    
    public:

        explicit Object(QObject* parent=0, const QString& name="");
        Object(const QVariantMap& data, QObject* parent=0);
        virtual ~Object();
        bool contains(qreal, qreal);
        QRect sceneRect() const;
        QRect contentRect() const;
        virtual void resize(int, int);
        virtual void resize(int, int, int);
        virtual void move(int x, int y);
        void dragMove(int x, int y);
        void stopMove();
        QList<QRect> previousSceneRects() const;
        QColor backgroundColor() const;
        void setBackgroundColor(const QColor&);
        void stopResizing();
        int percentWidth() const;
        int percentHeight() const;

        int borderWidth();
        void setBorderWidth(int);
        QColor borderColor();
        void setBorderColor(const QColor&);

        virtual void show();
        virtual void hide();

        void fixPoint(int, int, int);

        void setTemporaryBackgroundImage(ImageFile*);
        ImageFile* temporaryBackgroundImage() const;

        void setTemporaryBackgroundColor(const QColor&);
        QColor temporaryBackgroundColor() const;

        void setTemporaryBackgroundOpacity(int);
        int temporaryBackgroundOpacity() const;

        /*int backgroundColorOpacity() const;
        void setBackgroundColorOpacity(int);

        int backgroundImageOpacity() const;
        void setBackgroundImageOpacity(int);*/

        int backgroundOpacity() const;
        void setBackgroundOpacity(int);

        int opacity() const;
        qreal opacityF() const;
        virtual void setOpacity(int);

        int width() const;
        int height() const;
        int contentWidth() const;
        int contentHeight() const;
        virtual void setWidth(int, bool percent=false);
        virtual void setHeight(int, bool percent=false);
        virtual void setX(int);
        virtual void setY(int);
        int x() const;
        int y() const;
        int contentX() const;
        int contentY() const;

        bool isRounded() const;
        int cornerRadius();
        void setCornerRadius(int);

        ImageFile* backgroundImage() const;
        void setBackgroundImage(const QString&);

        bool visible();
        void setVisible(bool);

        void update();
        void setEventActions(Interaction::InputEvent, const QList<Action*> &);
        void removeEventActionAt(Interaction::InputEvent, int, bool del=false);
        void removeEventActions(Interaction::InputEvent, bool del=false);
        QList<Action*> actionsForEvent(Interaction::InputEvent);
        Action* eventAction(Interaction::InputEvent, const QString&);
        bool hasActionForEvent(Action*, Interaction::InputEvent);
        void moveSharedEventActions(Object*, Object*, Interaction::InputEvent);
        void moveAllSharedEventActions(Object*, Object*);
        virtual void paint(QPainter&);
        Object* copy();
        virtual QVariantMap toJsonObject(bool internal=true) const;
        bool containsResizeRectAt(int, int);
        QRect resizeRectAt(int, int);
        QList<QRect> resizeRects();
        void updateResizeRect(int, const QPoint&);
        void setHoveredResizeRect(int);
        virtual Object* objectAt(qreal, qreal);
        void selectObjectAt(qreal, qreal);
        Object* selectedObject();

        Padding padding();
        int padding(const QString&);
        void setPadding(const Padding&);
        void setPadding(const QString&, int);

        int paddingLeft() const;
        void setPaddingLeft(int);

        int paddingTop() const;
        void setPaddingTop(int);

        int paddingRight() const;
        void setPaddingRight(int);

        int paddingBottom() const;
        void setPaddingBottom(int);

        void filterResourceData(QVariantMap&) const;

        bool hasObjectAsParent();
        bool keepAspectRatio();
        void setKeepAspectRatio(bool);

        static QString defaultFontFamily();
        static void setDefaultFontFamily(const QString&);
        static int defaultFontSize();
        static void setDefaultFontSize(int);
        static QFont defaultFont();
        static void setDefaultFont(const QFont&);

    public slots:
        void onResizeEvent(QResizeEvent*);
        void onParentResized(int, int);
        void appendEventAction(Interaction::InputEvent, Action*);
        void insertEventAction(Interaction::InputEvent, int, Action*);
        void removeEventAction(Interaction::InputEvent, Action *, bool del=false);

    private slots:
        void removeEventActionSync(Interaction::InputEvent, Action *, bool del=false);

//    private slots:
//        void eventActionChanged();

    signals:
        void eventActionAdded(Interaction::InputEvent, Action*);
        void eventActionRemoved(Interaction::InputEvent, Action*, bool);
        void positionChanged(int, int);
        void resized(int, int);
        void destroyed(Object* object);
        void synced();
        void eventActionInserted(Interaction::InputEvent, int, Action*);

    private:
        //void init(const QString &, int, int, QObject*);
        void init();
        void updateResizeRects();
        void movePoint(int, QPoint&);
        int parentWidth() const;
        int parentHeight() const;
        void updateScaledBackgroundImage();
        void replaceEventActions(Interaction::InputEvent, const QList<Action*> &);
        void addEventActions(Interaction::InputEvent, const QVariantList&);
        void clearEventActions(Interaction::InputEvent, const QList<Action*>&);
        void connectEventActions(Interaction::InputEvent, Object*);
        void copyResourceActions(Interaction::InputEvent);

    protected:
        QRect mSceneRect;
        QRect mBoundingRect;
        QRect mPreviousSceneRect;
        QRect mPreviousResizeRect;
        int mPercentWidth;
        int mPercentHeight;
        float mXDif;
        float mYDif;
        Padding mPadding;
        QList<QRect> mPreviousSceneRects;
        QHash<Interaction::InputEvent, QList<Action*> > mEventToActions;
        void updateAspectRatio();
        virtual float calculateAspectRatio();
        virtual void connectToResource();
        virtual void loadData(const QVariantMap&, bool internal=false);
        virtual void filterLoadData(QVariantMap&);
        virtual void resizeSceneRect(int x, int y);

    private: //variables
        QList<QRect> mResizeRects;
        bool mVisible;
        int mOriginalResizePointIndex;
        float mAspectRatio;
        int mOriginalWidth;
        int mBorderWidth;
        int mOpacity;
        QColor mBorderColor;
        Object* mSelectedObject;
        bool mKeepAspectRatio;
        int mCornerRadius;
        QPixmap* mScaledBackgroundImage;
        Background mBackground;
        Background mTemporaryBackground;
};


#endif
