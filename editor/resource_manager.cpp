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

#include "resource_manager.h"

#include <QDebug>
#include <QMovie>

#include "utils.h"
#include "image.h"
#include "character.h"
#include "dialoguebox.h"
#include "textbox.h"
#include "button.h"
#include "menu.h"
#include "animationimage.h"

static QList<Object*> mResources;
static ResourceManager* mInstance = new ResourceManager();
static QHash<QString, ImageFile*> mImageCache;
static QHash<ImageFile*, int> mImageReferences;
static QHash<QString, int> mFontsPaths;
static QHash<QString, QString> mMediaResources;
static QHash<QString, QString> mSounds;
static QString mRelativePath = "";

ResourceManager::ResourceManager(QObject *parent) :
    QObject(parent)
{
}

void ResourceManager::addResource(Object * obj)
{
    if (obj) {
        if (! isValidName(obj->objectName()))
           obj->setObjectName(newName(obj));

        if (! obj->parent())
            obj->setParent(this);

        connect(obj, SIGNAL(dataChanged()), this, SIGNAL(resourceChanged()));
        mResources.append(obj);
        emit resourceAdded(obj);
    }
}

bool ResourceManager::isValidName(const QString& name)
{
    if (name.isEmpty() || name.isNull())
        return false;

    for(int i=0; i < mResources.size(); i++)
        if (mResources[i]->objectName() == name)
            return false;

    return true;
}

QString ResourceManager::newName(QString name)
{
    if (name.isEmpty() || name.isNull())
        name = "object";

    while(! isValidName(name)) {
        name = Utils::incrementLastNumber(name);
    }

    return name;
}

QString ResourceManager::newName(Object* obj)
{
   QString name = obj->objectName();
   if (name.isEmpty() || name.isNull())
        name = obj->type();

   return newName(name);
}

QList<Object*> ResourceManager::resources()
{
    return mResources;
}

Object * ResourceManager::typeToObject(const QString& type, QVariantMap& data, QObject* parent)
{
    QString _type = type.toLower();

    if (_type == "object")
        return new Object(data, parent);
    else if (_type == "image")
        return new Image(data, parent);
    else if (_type == "character")
        return new Character(data, parent);
    else if (_type == "dialoguebox")
        return new DialogueBox(data, parent);
    else if (_type == "textbox")
        return new TextBox(data, parent);
    else if (_type == "button")
        return new Button(data, parent);
    else if (_type == "objectgroup")
        return new ObjectGroup(data, parent);
    else if (_type == "menu")
        return new Menu(data, parent);
    return 0;

}

Object* ResourceManager::createResource(QVariantMap data, bool appendToResources, QObject* parent)
{
    Object* _resource = 0;
    QString type("");

    fillWithResourceData(data);

    if (data.contains("resource") && data.value("resource").type() == QVariant::String)
        _resource = resource(data.value("resource").toString());

    if (data.contains("type") && data.value("type").type() == QVariant::String)
        type = data.value("type").toString();

    Object *object = typeToObject(type, data, parent);
    if (object && _resource)
        object->setResource(_resource);

    if (appendToResources)
        addResource(object);
    return object;
}

void ResourceManager::removeResource(Object *object, bool del)
{
    if (mResources.contains(object)) {
        mResources.removeOne(object);
        object->disconnect(this);
        emit resourceRemoved(object);
        if (del && object)
            object->deleteLater();
    }
}

void ResourceManager::fillWithResourceData(QVariantMap& data)
{
    if (! data.contains("resource") || data.value("resource").type() != QVariant::String)
        return;

    QString name = data.value("resource").toString();

    Object* resource = ResourceManager::resource(name);
    if (! resource)
        return;

    QVariantMap resourceData = resource->toJsonObject();
    QStringList keys = resourceData.keys();
    foreach(const QString& key, keys) {
        if (! data.contains(key))
            data.insert(key, resourceData.value(key));
    }
}

Object *ResourceManager::resource(const QString & name)
{
    for(int i=0; i < mResources.size(); i++) {
        if (mResources[i]->objectName() == name)
            return mResources[i];
    }

    return 0;
}

Object *ResourceManager::resource(int index)
{
    if (index >= mResources.size() || index < 0)
        return 0;

    return mResources.at(index);
}


bool ResourceManager::contains(const QString & name)
{
    if ( resource(name) )
        return true;
    return false;
}

void  ResourceManager::removeResources(bool del)
{
    for(int i=mResources.size()-1; i >= 0; i--)
        removeResource(mResources[i], del);

    mResources.clear();

    QHashIterator <QString, ImageFile*>it(mImageCache);
    ImageFile* image = 0;
    while(it.hasNext()) {
        it.next();
        image = static_cast<ImageFile*>(it.value());
        if (image)
            delete image;
    }

    mImageCache.clear();
    mImageReferences.clear();
}

void  ResourceManager::destroy()
{
    mInstance->removeResources(true);
    mInstance->deleteLater();
}

ResourceManager* ResourceManager::instance()
{
    return mInstance;
}

QString ResourceManager::absolutePath(const QString& path)
{
    if (! mRelativePath.isEmpty()) {
         QDir dir(mRelativePath);
         if (dir.exists(path))
             return dir.absoluteFilePath(path);
    }
    return path;
}

ImageFile* ResourceManager::newImage(const QVariant& imageData)
{
    if (imageData.type() == QVariant::String) {
        return newImage(imageData.toString());
    }
    else if (imageData.type() == QVariant::Map) {
        QVariantMap imageMap = imageData.toMap();
        if (imageMap.contains("src"))
            return newImage(imageMap.value("src").toString());
    }

    return 0;
}

ImageFile* ResourceManager::newImage(const QString& fileName)
{
    QString path;
    //fileName can be either a full path or just the file name
    if (mMediaResources.contains(fileName))
        path = ResourceManager::mediaPath(fileName);
    else
        path = ResourceManager::absolutePath(fileName);

    if (mImageCache.contains(path)) {
        incrementReference(mImageCache.value(path));
        return mImageCache.value(path);
    }

    if (! QFile::exists(path))
        return 0;

    ImageFile* image = ImageFile::create(path);
    image->setName(newMedia(path));
    mImageCache.insert(path, image);
    mImageReferences.insert(image, 1);

    return image;
}

QString ResourceManager::imagePath(ImageFile* image)
{
    if (! image)
        return "";

    QHashIterator<QString, ImageFile*> it(mImageCache);

    while(it.hasNext()) {
        it.next();
        if (it.value() == image)
            return it.key();
    }

    return "";
}

ImageFile* ResourceManager::image(const QString& path)
{
    if (path.isEmpty())
        return 0;

    if (mImageCache.contains(path))
        return mImageCache.value(path);

    return 0;
}

void ResourceManager::incrementReference(ImageFile* image)
{
    int refs = mImageReferences.value(image, -1);
    if (refs == -1)
        return;
    refs++;
    mImageReferences.insert(image, refs);
}


void ResourceManager::decrementReference(const QPixmap& _pixmap)
{
    QHashIterator<QString, ImageFile*> it(mImageCache);
    ImageFile* image = 0;
    while(it.hasNext()) {
        it.next();
        if (it.value()->pixmap().toImage() == _pixmap.toImage()) {
            image = it.value();
            break;
        }
    }

    decrementReference(image);
}

void ResourceManager::decrementReference(ImageFile* image)
{
    if(! image || ! mImageReferences.contains(image))
        return;

    int refs = mImageReferences.value(image);
    if (refs <= 1)
    {
        mImageReferences.remove(image);

        QString keyPath = ResourceManager::imagePath(image);
        if (! keyPath.isEmpty())
            mImageCache.remove(keyPath);

        delete image;
    }
    else {
        refs--;
        mImageReferences.insert(image, refs);
    }
}

QStringList ResourceManager::imagePaths()
{
    return mImageCache.keys();
}

void ResourceManager::setRelativePath(const QString & path)
{
    mRelativePath = path;
}

int ResourceManager::newFont(const QString& p)
{
    QString path = ResourceManager::absolutePath(p);
    if (mFontsPaths.contains(path))
        return mFontsPaths.value(path, -1);

    int id = QFontDatabase::addApplicationFont(path);
    mFontsPaths.insert(path, id);
    ResourceManager::newMedia(path);
    return id;
}

QString ResourceManager::newMedia(const QString& path)
{
    QString absPath = absolutePath(path);
    QFileInfo info(absPath);
    QString name = info.fileName();

    if (mMediaResources.contains(name)) {
        if (absPath == mMediaResources.value(name))
            return name; //file already added, do nothing

        while(mMediaResources.contains(name) && mMediaResources.value(name) != absPath)
            name = Utils::incrementLastNumber(name);
    }

    mMediaResources.insert(name, absPath);
    return name;
}

QString ResourceManager::mediaPath(const QString& name)
{
    if (mMediaResources.contains(name))
        return mMediaResources.value(name);
    return "";
}

QString ResourceManager::mediaName(const QString& path)
{
    QHashIterator<QString, QString> it(mMediaResources);
    while(it.hasNext()) {
        it.next();
        if (it.value() == path)
            return it.key();
    }
    return "";
}

void ResourceManager::exportResources(const QDir& dir)
{
    ResourceManager::exportCustomFonts(dir);

    //export everything else (e.g. sounds)
    QHashIterator<QString, QString> it(mMediaResources);
    while(it.hasNext()) {
        it.next();
        if (! QFile::exists(dir.absoluteFilePath(it.key())))
            QFile::copy(it.value(), dir.absoluteFilePath(it.key()));
    }
}

void ResourceManager::exportCustomFonts(const QDir& dir)
{
    QHashIterator<QString, int> it(mFontsPaths);
    QFile file(dir.absoluteFilePath("fontfaces.css"));
    if (! file.open(QFile::WriteOnly | QFile::Text))
        return;

    while(it.hasNext()) {
        it.next();
        QString fontName = ResourceManager::mediaName(it.key());
        if (fontName.isEmpty())
            fontName = QFileInfo(it.key()).baseName();

        //copy font file
        QFile::copy(it.key(), dir.absoluteFilePath(fontName));

        //determine font's family name.
        QStringList families = QFontDatabase::applicationFontFamilies(it.value());
        if (families.size() == 1)
            fontName = families[0];

        //write css for font
        file.write(Utils::fontFace(fontName).toAscii());
        file.write("\n");
    }

    file.close();
}

void ResourceManager::importResources(const QVariantMap& data)
{
    if (data.contains("resources") && data.value("resources").type() == QVariant::Map) {
        QVariantMap resourcesMap = data.value("resources").toMap();
        QMapIterator<QString, QVariant> it(resourcesMap);
        while(it.hasNext()) {
            it.next();
            if (it.value().type() != QVariant::Map)
                continue;

            ResourceManager::instance()->createResource(it.value().toMap());
        }
    }

    if (data.contains("customFonts") && data.value("customFonts").type() == QVariant::List) {
        QVariantList fonts = data.value("customFonts").toList();
        foreach(const QVariant& font, fonts){
            newFont(font.toString());
        }
    }
}

QString ResourceManager::display()
{
    QString display = "canvas";
    foreach(const QString& path, mImageCache.keys()) {
        ImageFile* image = mImageCache[path];
        if (image && image->isAnimated()) {
            display = "DOM";
            break;
        }
    }

    return display;
}

int ResourceManager::customFontsCount()
{
    return mFontsPaths.keys().size();
}

QList<int> ResourceManager::customFontsIds()
{
    return mFontsPaths.values();
}

QStringList ResourceManager::customFonts()
{
    QStringList fonts;
    QHashIterator<QString, int> it(mFontsPaths);
    while(it.hasNext()) {
        it.next();
        QFileInfo info(it.key());
        fonts.append(info.fileName());
    }

    return fonts;
}

QString ResourceManager::newSound(const QString& path)
{
    QString name = ResourceManager::newMedia(path);
    mSounds.insert(name, path);
    return name;
}

QStringList ResourceManager::sounds()
{
    return mSounds.keys();
}
