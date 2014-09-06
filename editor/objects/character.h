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

#ifndef CHARACTER_H
#define CHARACTER_H

#include "image.h"
#include "character_properties_widget.h"


#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>

class CharacterPropertiesWidget;
class ResourceManager;

class Character : public Image
{
    Q_OBJECT

    QColor mNameColor;
    QColor mTextColor;
    QHash <QString, QString> mStateToPath;
    QHash <QString, ImageFile*> mStateToImage;
    QString mCurrentState;
    bool mAvailable;


    public:
        explicit Character(QObject* parent=0);
        Character(const QString& name, const QHash<QString, QString>& statesAndImagePaths, QObject* parent=0);
        Character(const QVariantMap&, QObject*);
        ~Character();
        void removeState(const QString&);
        //void setDefaulState(const QString&);
        //QString defaultState() const;
        static CharacterPropertiesWidget* characterEditorWidget();
        static void setCharacterEditorWidget(CharacterPropertiesWidget *);
        virtual ObjectEditorWidget* editorWidget();
        QHash <QString, QString> statesToPaths();
        void setAvailable(bool);
        bool isAvailable();
        void setCurrentState(const QString&);
        QString currentState() const;
        void setStates(const QHash<QString, QString>&);
        QColor nameColor() const;
        QColor textColor() const;
        void setNameColor(const QColor&);
        void setTextColor(const QColor&);
        virtual QVariantMap toJsonObject(bool internal=true);

   private:
        void init(const QString &);
        void initStates();
};


#endif
