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

#include "engine.h"

#include <QStringList>
#include <QDir>
#include <QObject>
#include <QMessageBox>
#include <QCoreApplication>

static QString mDefaultPath = ENGINE_DEFAULT_PATH;
static QString mPath = mDefaultPath;
static QString mBrowserPath = "";
static bool mUseBuiltinBrowser = false;
static bool mPathChanged = false;

bool Engine::isValidPath(const QString & path)
{
    if (path.isEmpty() || path.isNull())
        return false;

    QDir dir(path);

    QStringList engineComponents;
    engineComponents << "index.html" << "style.css"  << "asset_manager.js" << "frame.js" << "init.js" << "main.js"
                     << "actions.js" << "objects.js" << "scene.js" << "utils.js" << "game.js" << "game_model.js" << "game_controller.js"
                     << "buzz.js" << "sound_manager.js" << "views.js" << "graphics.js" << "jquery.js" << "jstorage.js" << "fontfaceobserver.js";

    //check if all necessary files seem to be present
    QStringList missingFiles;

    foreach(const QString& file, engineComponents)
        if (! dir.exists(file))
            missingFiles << file;

    if (! missingFiles.isEmpty())
        return false;
    return true;
}

void Engine::setPath(const QString & path)
{
    if (path != mPath)
        mPathChanged = true;
    mPath = path;
}

bool Engine::pathChanged()
{
    bool changed = mPathChanged;
    mPathChanged = false;
    return changed;
}

QString Engine::path()
{
    return mPath;
}

bool Engine::loadPath()
{
    bool defaultLoaded = loadDefaultPath();

    if (isValidPath(mPath))
        return true;

    if (defaultLoaded) {
        mPath = mDefaultPath;
        return isValidPath(mPath);
    }

    return false;
}

bool Engine::isValid()
{
    return isValidPath(mPath);
}

QString Engine::defaultPath()
{
    return mDefaultPath;
}

bool Engine::loadDefaultPath()
{
    #if defined(Q_OS_UNIX)
        if (mDefaultPath == ENGINE_DEFAULT_PATH) {
            mDefaultPath = QCoreApplication::applicationDirPath() +
                           QDir::separator() + ENGINE_DEFAULT_PATH;
        }
    #endif

    if (isValidPath(mDefaultPath))
        return true;

    QStringList paths;
    paths << "engine";

    foreach(const QString& path, paths) {
        if (isValidPath(path)) {
            mDefaultPath = path;
            return true;
        }
    }

    return false;
}

QString Engine::browserPath()
{
    return mBrowserPath;
}

void Engine::setBrowserPath(const QString& path)
{
    mBrowserPath = path;
}

bool Engine::useBuiltinBrowser()
{
    return mUseBuiltinBrowser;
}

void Engine::setUseBuiltinBrowser(bool use)
{
    mUseBuiltinBrowser = use;
}
