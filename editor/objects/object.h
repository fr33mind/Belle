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

#include "action.h"
#include "interaction.h"
#include "object_editor_widget.h"
#include "padding.h"
#include "animatedimage.h"
#include "background.h"
#include "assetmanager.h"

#define RESIZE_RECT_WIDTH 7

class Scene;
class ObjectEditorWidget;
class Action;

class Object : public QObject
{
    Q_OBJECT
    
    public:

        explicit Object(QObject* parent=0, const QString& name="Object");
        Object(const QVariantMap& data, QObject* parent=0);
        virtual ~Object();
        bool contains(qreal, qreal);
        QRect sceneRect() const;
        virtual void resize(int, int);
        virtual void resize(int, int, int);
        virtual void move(int x, int y);
        void dragMove(int x, int y);
        void stopMove();
        QList<QRect> previousSceneRects() const;
        static ObjectEditorWidget* objectEditorWidget();
        static void setObjectEditorWidget(ObjectEditorWidget*);
        virtual ObjectEditorWidget* editorWidget();
        QColor backgroundColor() const;
        void setBackgroundColor(const QColor&);
        void stopResizing();
        int percentWidth() const;
        int percentHeight() const;
        Scene* scene() const;
        bool isValidName(const QString&);

        int borderWidth();
        void setBorderWidth(int);
        QColor borderColor();
        void setBorderColor(const QColor&);

        void setEditableName(bool);
        bool editableName();

        virtual void show();
        virtual void hide();

        void fixPoint(int, int, int);

        /*int backgroundColorOpacity() const;
        void setBackgroundColorOpacity(int);

        int backgroundImageOpacity() const;
        void setBackgroundImageOpacity(int);*/

        int backgroundOpacity() const;
        void setBackgroundOpacity(int);

        int opacity() const;
        qreal opacityF() const;
        void setOpacity(int);

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

        bool isRounded() const;
        int cornerRadius();
        void setCornerRadius(int);

        ImageFile* backgroundImage() const;
        void setBackgroundImage(const QString&);

        bool visible();
        void setVisible(bool);

        QString type();
        void setType(const QString&);
        void update();
        void setEventActions(Interaction::InputEvent, const QList<Action*> &);
        void appendEventAction(Interaction::InputEvent, Action*);
        void insertEventAction(Interaction::InputEvent, int, Action*);
        void removeEventActionAt(Interaction::InputEvent, int, bool del=false);
        void removeEventAction(Interaction::InputEvent, Action*, bool del=false);
        void removeAllEventActions(Interaction::InputEvent, bool del=false);
        QList<Action*> actionsForEvent(Interaction::InputEvent);
        bool hasActionForEvent(Action*, Interaction::InputEvent);
        void moveSharedEventActions(Object*, Object*, Interaction::InputEvent);
        void moveAllSharedEventActions(Object*, Object*);
        virtual void paint(QPainter&);
        Object* copy();
        virtual QVariantMap toJsonObject(bool internal=true);
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

        void filterResourceData(QVariantMap&);
        QVariantMap fillWithResourceData(QVariantMap);

        void setResource(Object*);
        Object* resource() const;

        void addClone(Object*);
        void removeClone(Object*);
        QList<Object*> clones() const;

        void addObserver(Object*);
        void removeObserver(Object*);

        bool changesAccepted() const;
        bool changesPropagated() const;

        bool isResource() const;
        void blockNotifications(bool);

        bool setName(const QString&);
        QString name();

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
        virtual void load(const QVariantMap &);
        void onParentResized(int, int);
        void acceptChanges(bool);
        void propagateChanges(bool);

    signals:
        void dataChanged(const QVariantMap& data=QVariantMap());
        void positionChanged(int, int);
        void resized(int, int);
        void destroyed(Object* object=0);

    private:
        //void init(const QString &, int, int, QObject*);
        void init(const QString&);
        void updateResizeRects();
        void movePoint(int, QPoint&);
        int parentWidth() const;
        int parentHeight() const;
        void updateScaledBackgroundImage();
        void _load(const QVariantMap&);
        void replaceEventActions(Interaction::InputEvent, const QList<Action*> &);

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
        bool mEditableName;
        Object* mResource;
        void notify(const QString&, const QVariant&, const QVariant& prev=QVariant());
        void updateAspectRatio();

    private: //variables
        QString mType;
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
        QList<Object*> mClones;
        bool mChangesAccepted;
        bool mChangesPropagated;

private slots:
        void resourceDestroyed();
        void cloneDestroyed(Object*);

};


#endif
