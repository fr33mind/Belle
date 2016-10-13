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

#include "changecolor.h"

#include "resource_manager.h"
#include "utils.h"

ChangeColor::ChangeColor(QObject *parent) :
    Action(parent)
{
    init();
    Object * object = qobject_cast<Object*>(parent);
    if (object)
        setSceneObject(object);
}

ChangeColor::ChangeColor(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("color") && data.value("color").type() == QVariant::List)
        setColor(Utils::listToColor(data.value("color").toList()));

    if (data.contains("colorChangeEnabled") && data.value("colorChangeEnabled").type() == QVariant::Bool)
        setColorChangeEnabled(data.value("colorChangeEnabled").toBool());

    if (data.contains("image") && data.value("image").type() == QVariant::String)
        setImage(data.value("image").toString());

    if (data.contains("imageChangeEnabled") && data.value("imageChangeEnabled").type() == QVariant::Bool)
        setImageChangeEnabled(data.value("imageChangeEnabled").toBool());

    if (data.contains("opacity") && data.value("opacity").canConvert(QVariant::Int))
        setOpacity(data.value("opacity").toInt());

    if (data.contains("opacityChangeEnabled") && data.value("opacityChangeEnabled").type() == QVariant::Bool)
        setOpacityChangeEnabled(data.value("opacityChangeEnabled").toBool());
}

ChangeColor::~ChangeColor()
{
    restoreSceneObject();
    releaseImage();
}

void ChangeColor::init()
{
    setType(GameObjectMetaType::ChangeColor);
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease | Interaction::MouseMove);
    mColor = Qt::white;
    mImage = 0;
    mImageChangeEnabled = true;
    mColorChangeEnabled = true;
    mOpacityChangeEnabled = false;
}

QVariantMap ChangeColor::toJsonObject(bool internal) const
{
    QVariantMap action = Action::toJsonObject(internal);

    action.insert("color", Utils::colorToList(mColor));
    action.insert("colorChangeEnabled", mColorChangeEnabled);

    QVariant name;
    if (mImage)
        name = mImage->name();
    action.insert("image", name);
    action.insert("imageChangeEnabled", mImageChangeEnabled);

    action.insert("opacity", opacity());
    action.insert("opacityChangeEnabled", mOpacityChangeEnabled);

    return action;
}

int ChangeColor::opacity() const
{
    return mColor.alpha();
}

void ChangeColor::setOpacity(int a)
{
    if (mColor.alpha() == a)
        return;

    mColor.setAlpha(a);

    Object* obj = sceneObject();
    if (obj)
        obj->setTemporaryBackgroundOpacity(a);

    emit dataChanged();
}

QColor ChangeColor::color() const
{
    return mColor;
}

void ChangeColor::setColor(const QColor & color)
{
    if (mColor == color)
        return;

    int alpha = opacity();
    mColor = color;
    mColor.setAlpha(alpha);

    Object* obj = sceneObject();
    if (obj)
        obj->setTemporaryBackgroundColor(mColor);

    emit dataChanged();
}

ImageFile* ChangeColor::image() const
{
    return mImage;
}

void ChangeColor::setImage(const QString& path)
{
    ImageFile* image = dynamic_cast<ImageFile*>(AssetManager::instance()->loadAsset(path, Asset::Image));
    if (mImage == image) {
        AssetManager::instance()->releaseAsset(image);
        return;
    }

    setImage(image);
    mReleaseImage = true;
}

void ChangeColor::setImage(ImageFile* image)
{
    if (mImage == image)
        return;

    Object* obj = sceneObject();
    if (obj)
        obj->setTemporaryBackgroundImage(image);

    releaseImage();
    mImage = image;
    emit dataChanged();
}

QString ChangeColor::displayText() const
{
    Object* target = sceneObject();
    QString text;

    if (!target)
        return text;

    QString name = target->name();
    QString opacity_str;
    if (mOpacityChangeEnabled) {
        double op = opacity() * 100.0 / 255;
        opacity_str = QString("(%1%)").arg(QString::number(op, 'f', 1));
    }

    if (mImage && mImageChangeEnabled)
        text = tr("\"%1\" to \"%2\" %3").arg(name, mImage->name(), opacity_str);
    else if (mColor.isValid() && mColorChangeEnabled)
        text = tr("\"%1\" to %2 %3").arg(name, mColor.name(QColor::HexRgb), opacity_str);

    return text;
}

void ChangeColor::loadSceneObject()
{
    Action::loadSceneObject();
    _loadSceneObject();
}

void ChangeColor::restoreSceneObject()
{
    Action::restoreSceneObject();

    Object* obj = sceneObject();
    if (obj) {
        obj->setTemporaryBackgroundImage(0);
        obj->setTemporaryBackgroundColor(QColor());
        obj->setTemporaryBackgroundOpacity(0);
    }
}

void ChangeColor::_loadSceneObject()
{
    Object* obj = sceneObject();
    if (obj) {
        if (mImage) {
            if (mImageChangeEnabled)
                obj->setTemporaryBackgroundImage(mImage);
            else
                obj->setTemporaryBackgroundImage(0);
        }

        if (mColor.isValid()) {
            if (mColorChangeEnabled)
                obj->setTemporaryBackgroundColor(mColor);
            else
                obj->setTemporaryBackgroundColor(QColor());
        }

        if (mOpacityChangeEnabled)
            obj->setTemporaryBackgroundOpacity(mColor.alpha());
        else
            obj->setTemporaryBackgroundOpacity(obj->backgroundOpacity());
    }
}

void ChangeColor::releaseImage()
{
    if (mImage && mReleaseImage)
        AssetManager::instance()->releaseAsset(mImage);

    mImage = 0;
    mReleaseImage = false;
}

void ChangeColor::setImageChangeEnabled(bool enabled)
{
    mImageChangeEnabled = enabled;
    _loadSceneObject();
    emit dataChanged();
}

bool ChangeColor::isImageChangeEnabled() const
{
    return mImageChangeEnabled;
}

void ChangeColor::setColorChangeEnabled(bool enabled)
{
    mColorChangeEnabled = enabled;
    _loadSceneObject();
    emit dataChanged();
}

bool ChangeColor::isColorChangeEnabled() const
{
    return mColorChangeEnabled;
}

void ChangeColor::setOpacityChangeEnabled(bool enabled)
{
    mOpacityChangeEnabled = enabled;
    _loadSceneObject();
    emit dataChanged();
}

bool ChangeColor::isOpacityChangeEnabled() const
{
    return mOpacityChangeEnabled;
}
