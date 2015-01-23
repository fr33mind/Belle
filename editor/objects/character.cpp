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
#include "utils.h"
#include "resource_manager.h"

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
    setStates(statesAndImagePaths);
}

Character::Character(const QVariantMap& data, QObject* parent) :
    Image(data, parent)
{
    init(objectName());
    _load(data);
}

Character::~Character()
{
    QHashIterator<QString, ImageFile*> it(mStateToImage);
    while(it.hasNext()) {
        it.next();
        AssetManager::instance()->releaseAsset(it.value());
    }
    mStateToImage.clear();
}

void Character::_load(const QVariantMap& data)
{
    this->blockNotifications(true);

    QHash<QString, QString> states;
    if (data.contains("states") && data.value("states").type() == QVariant::Map) {
        QMapIterator<QString, QVariant> it(data.value("states").toMap());
        while(it.hasNext()) {
            it.next();
            if (it.value().type() == QVariant::String)
                states.insert(it.key(), it.value().toString());
        }

        setStates(states);
    }

    if (data.contains("state") && data.value("state").type() == QVariant::String) {
        setCurrentState(data.value("state").toString());
    }

    if (data.contains("nameColor") && data.value("nameColor").type() == QVariant::List)
        setNameColor(Utils::listToColor(data.value("nameColor").toList()));

    if (data.contains("textColor") && data.value("textColor").type() == QVariant::List)
        setTextColor(Utils::listToColor(data.value("textColor").toList()));

    this->blockNotifications(false);
}

void Character::load(const QVariantMap& data)
{
    Image::load(data);
    _load(data);
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
        ImageFile* image = dynamic_cast<ImageFile*>(AssetManager::instance()->loadAsset(it.value(), Asset::Image));
        mStateToImage.insert(it.key(), image);
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
        AssetManager::instance()->releaseAsset(image);
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

QHash <QString, ImageFile*> Character::statesToImages()
{
    return mStateToImage;
}

QStringList Character::states() const
{
    return mStateToPath.keys();
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

    this->notify("state", state);
}

QString Character::currentState() const
{
    return mCurrentState;
}

void Character::setStates(const QHash<QString, QString> & states)
{
    QList<ImageFile*> images = mStateToImage.values();
    mStateToImage.clear();
    mStateToPath = states;
    QVariantMap statesNotify;
    QHashIterator<QString, QString> it(states);
    while(it.hasNext()) {
        it.next();
        ImageFile* image = dynamic_cast<ImageFile*>(AssetManager::instance()->loadAsset(it.value(), Asset::Image));
        mStateToImage.insert(it.key(), image);
        statesNotify.insert(it.key(), it.value());
    }

    foreach(ImageFile* img, images){
        AssetManager::instance()->releaseAsset(img);
    }

    this->notify("states", statesNotify);

    if (mCurrentState.isEmpty() && ! mStateToPath.isEmpty())
        setCurrentState(mStateToPath.keys().first());
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
    if (mNameColor == color)
        return;

    mNameColor = color;
    this->notify("nameColor", Utils::colorToList(color));
}

void Character::setTextColor(const QColor & color)
{
    if (mTextColor == color)
        return;

    mTextColor = color;
    this->notify("textColor", Utils::colorToList(color));
}

QVariantMap Character::toJsonObject(bool internal)
{
    QVariantMap object = Object::toJsonObject(internal);
    QVariantMap stateToPath;
    QHashIterator<QString, ImageFile*> it(mStateToImage);

    while(it.hasNext()) {
        it.next();
        if (it.value()) {
            QString filename = it.value()->name();
            if (internal)
                filename = it.value()->path();
            stateToPath.insert(it.key(), filename);
        }
    }

    if (mNameColor.isValid())
        object.insert("nameColor", Utils::colorToList(mNameColor));
    if (mTextColor.isValid())
        object.insert("textColor", Utils::colorToList(mTextColor));

    object.insert("states", stateToPath);
    if (! mCurrentState.isEmpty())
        object.insert("state", mCurrentState);
    filterResourceData(object);

    return object;
}
