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

#include "character.h"

#include <QDebug>
#include <QFileInfo>

#include "scene.h"

static CharacterPropertiesWidget* mEditorWidget = 0;

Character::Character(QObject* parent) :
    Image(0, parent)
{
    init("");
}

Character::Character(const QString& name, const QHash<QString, QString>& statesAndImagePaths, QObject* parent) :
    Image(0, parent)
{
    init(name);

    mStateToPath = statesAndImagePaths;
    initStates();
    if (! statesAndImagePaths.isEmpty())
        setCurrentState(statesAndImagePaths.keys().first());
}

Character::Character(const QVariantMap& data, QObject* parent) :
    Image(data, parent)
{
    init(objectName());

    if (data.contains("images") && data.value("images").type() == QVariant::Map) {
        QMapIterator<QString, QVariant> it(data.value("images").toMap());
        while(it.hasNext()) {
            it.next();
            if (it.value().type() == QVariant::String)
                mStateToPath.insert(it.key(), it.value().toString());
        }
    }

    initStates();

    if (! mStateToPath.isEmpty())
        setCurrentState(mStateToPath.keys().first());
}

Character::~Character()
{
    QHashIterator<QString, ImageFile*> it(mStateToImage);
    while(it.hasNext()) {
        it.next();
        ResourceManager::decrementReference(it.value());
    }
    mStateToImage.clear();
}

void Character::init(const QString& name)
{
    mAvailable = true;
    mCurrentState = "";
    mNameColor = QColor(Qt::white);
    mTextColor = QColor(Qt::white);
    setType("Character");
    if (! name.isEmpty())
        setObjectName(name);
}

void Character::initStates()
{
    QHashIterator<QString, QString> it(mStateToPath);
    while(it.hasNext()) {
        it.next();
        mStateToImage.insert(it.key(), ResourceManager::newImage(it.value()));
    }
}

void Character::removeState(const QString& state)
{
    QString path = "";
    if (! mStateToPath.contains(state) )
            return;

    path = mStateToPath.take(state);
    if (state == mCurrentState)
        setImage(0);

    if ((! path.isEmpty()) && mStateToImage.contains(path)) {
        ImageFile * image = mStateToImage.take(path);
        ResourceManager::decrementReference(image);
    }
}

/*void Character::setDefaulState(const QString& state)
{
    if (! mStateToPath.contains(state) || mCurrentState == state)
        return;

    mCurrentState = state;
    if (! mStateToImage.contains(mCurrentState))
        mStateToImage.insert(mCurrentState, new QPixmap(mStateToPath.value(mCurrentState)));

    setImage(mStateToImage.value(mCurrentState));

    if (! image()->isNull()) {
        mSceneRect.setWidth(image()->width());
        mSceneRect.setHeight(image()->height());
    }

    emit dataChanged();
}

QString Character::defaultState() const
{
    return mCurrentState;
}*/

CharacterPropertiesWidget* Character::characterEditorWidget()
{
    return mEditorWidget;
}

void Character::setCharacterEditorWidget(CharacterPropertiesWidget* widget)
{
    if (! widget)
        return;

    if (mEditorWidget)
        mEditorWidget->deleteLater();

    mEditorWidget = widget;
}

ObjectEditorWidget* Character::editorWidget()
{
    return mEditorWidget;
}

QHash <QString, QString> Character::statesToPaths()
{
    return mStateToPath;
}

void Character::setAvailable(bool available)
{
    mAvailable = available;
}

bool Character::isAvailable()
{
    return mAvailable;
}

void Character::setCurrentState(const QString & state)
{
    QString path = mStateToPath.value(state);
    if (path.isEmpty())
        return;

    mCurrentState = state;

    if (mStateToImage.contains(state))
        setImage(mStateToImage.value(state));
    else {
        setImage(path);
        mStateToImage.insert(state, image());
    }

    emit dataChanged();
}

QString Character::currentState() const
{
    return mCurrentState;
}

void Character::addStates(const QHash<QString, QString> & states)
{
    QHashIterator<QString, QString> it(states);
    while (it.hasNext()) {
        it.next();
        mStateToPath.insert(it.key(), it.value());
    }
}

QColor Character::nameColor() const
{
    return mNameColor;
}

QColor Character::textColor() const
{
    return mTextColor;
}

void Character::setNameColor(const QColor & color)
{
    mNameColor = color;
}

void Character::setTextColor(const QColor & color)
{
    mTextColor = color;
}

QVariantMap Character::toJsonObject(bool internal)
{
    QVariantMap object = Object::toJsonObject(internal);
    QVariantMap stateToPath;
    QHashIterator<QString, ImageFile*> it(mStateToImage);

    while(it.hasNext()) {
        it.next();        
        if (it.value())
            stateToPath.insert(it.key(), it.value()->fileName());
    }

    object.insert("images", stateToPath);
    filterResourceData(object);

    return object;
}
