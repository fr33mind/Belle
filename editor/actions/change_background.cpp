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

#include "change_background.h"
#include "scene_manager.h"
#include "utils.h"
#include "assetmanager.h"

#include <QFileInfo>
#include <QtDebug>

ChangeBackground::ChangeBackground(QObject *parent) :
    Action(parent)
{
    init();
}

ChangeBackground::ChangeBackground(const QVariantMap & data, QObject *parent):
    Action(data, parent)
{
    init();
    loadInternal(data);
}

ChangeBackground::~ChangeBackground()
{
    AssetManager::instance()->releaseAsset(mBackgroundImage);
}

void ChangeBackground::init()
{
    setType(GameObjectMetaType::ChangeBackground);

    mBackgroundImage = 0;
}

void ChangeBackground::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    if (data.contains("backgroundImage") && data.value("backgroundImage").type() == QVariant::String)
        setBackgroundImage(data.value("backgroundImage").toString());

    if (data.contains("backgroundColor") && data.value("backgroundColor").type() == QVariant::List)
        setBackgroundColor(Utils::listToColor(data.value("backgroundColor").toList()));
}

void ChangeBackground::setBackgroundImage(const QString & background)
{ 
    ImageFile* image = dynamic_cast<ImageFile*>(AssetManager::instance()->loadAsset(background, Asset::Image));
    if (mBackgroundImage == image) {
        AssetManager::instance()->releaseAsset(image);
        return;
    }

    AssetManager::instance()->releaseAsset(mBackgroundImage);
    mBackgroundImage = image;
    updateDisplayText();

    QString name = mBackgroundImage ? mBackgroundImage->name() : "";
    notify("backgroundImage", name);

    if (isActive()) {
        Scene* scene = this->scene();
        if (scene)
            scene->setTemporaryBackgroundImage(mBackgroundImage);
    }
}

ImageFile* ChangeBackground::background() const
{
    return mBackgroundImage;
}

QString ChangeBackground::backgroundPath()
{
    if (mBackgroundImage)
        return mBackgroundImage->path();
    return "";
}

void ChangeBackground::setBackgroundColor(const QColor& color)
{
    if (mBackgroundColor == color)
        return;

    mBackgroundColor = color;
    updateDisplayText();
    notify("backgroundColor", Utils::colorToList(color));

    if (isActive()) {
        Scene* scene = this->scene();
        if (scene)
            scene->setTemporaryBackgroundColor(color);
    }
}

QColor ChangeBackground::backgroundColor()
{
    return mBackgroundColor;
}

void ChangeBackground::loadSceneObject()
{
    Action::loadSceneObject();
    Scene *scene = this->scene();

    if (scene) {
        scene->setTemporaryBackgroundColor(mBackgroundColor);
        scene->setTemporaryBackgroundImage(mBackgroundImage);
    }
}

void ChangeBackground::restoreSceneObject()
{
    Action::restoreSceneObject();
    Scene* scene = this->scene();

    if (scene) {
        scene->setTemporaryBackgroundImage(0);
        scene->setTemporaryBackgroundColor(QColor());
    }
}

QVariantMap ChangeBackground::toJsonObject(bool internal) const
{
    QVariantMap data = Action::toJsonObject(internal);
    QString name = mBackgroundImage ? mBackgroundImage->name() : "";

    if (! name.isEmpty())
        data.insert("backgroundImage", name);
    if (mBackgroundColor.isValid())
        data.insert("backgroundColor", Utils::colorToList(mBackgroundColor));

    return data;
}

void ChangeBackground::updateDisplayText()
{
    QStringList textparts;
    if (mBackgroundImage)
        textparts.append(QString("\"%1\"").arg(mBackgroundImage->name()));
    if (mBackgroundColor.isValid())
        textparts.append(mBackgroundColor.name());
    setDisplayText(textparts.join(" and "));
}
