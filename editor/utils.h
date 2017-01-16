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

#ifndef UTILS_H
#define UTILS_H

#include <QColor>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QDebug>

namespace Utils
{

inline QColor listToColor(const QVariantList& color)
{
    if (color.size() < 3)
        return QColor();

    bool ok = true;

    int red = color.at(0).toInt(&ok);
    if (! ok)
        return QColor();

    int green = color.at(1).toInt(&ok);
    if (! ok)
        return QColor();

    int blue = color.at(2).toInt(&ok);
    if (! ok)
        return QColor();

    if (color.size() == 4) {
        int alpha = color.at(3).toInt(&ok);
        if (ok)
            return QColor(red, green, blue, alpha);
    }

    return QColor(red, green, blue, 255);
}

inline QVariantList colorToList(const QColor& color)
{
    QVariantList _color;
    if (! color.isValid())
        return _color;
    _color << color.red() << color.green() << color.blue()
             << color.alpha();
    return _color;
}

inline QString colorToString(const QColor& color)
{
    if (color.isValid()) {
        return QString("(%1, %2, %3, %4)").arg(color.red())
                                        .arg(color.green())
                                        .arg(color.blue())
                                        .arg(color.alpha());
    }

    return "()";
}

inline QString incrementLastNumber(const QString& text)
{
    int j;
    for(j=text.size()-1; j >= 0 && text[j].isDigit(); --j);
    j = j == text.size()-1 ? -1 : j+1;

    if (j == -1)
        return text + "1";
    else {
        int number = text.mid(j).toInt();
        number++;
        return text.left(j) + QString::number(number);
    }

    return text;
}

inline QString incrementFileName(const QString& fileName)
{
    QFileInfo info(fileName);
    QString name = incrementLastNumber(info.baseName());
    if (!info.completeSuffix().isEmpty())
        name += "." + info.completeSuffix();
    return name;
}

inline QString newFilePath(const QString& path)
{
    if (path.isEmpty() || path.isNull() || ! QFile::exists(path))
        return path;

    QFileInfo info(path);
    QString name(info.baseName());
    QDir dir = info.dir();
    QString suffix("");
    if (! info.completeSuffix().isEmpty())
        suffix = "." + info.completeSuffix();

    while(dir.exists(name+suffix)) {
        name = Utils::incrementLastNumber(name);
    }

    return dir.absoluteFilePath(name+suffix);
}

inline QString newFilePath(const QDir& dir, const QString& name)
{
    return newFilePath(dir.absoluteFilePath(name));
}

inline QString newFileName(const QString& path)
{    
    QString _path = newFilePath(path);
    return QFileInfo(_path).fileName();
}

inline QString newFileName(const QDir& dir, const QString& name)
{
    return newFileName(dir.absoluteFilePath(name));
}

inline void safeCopy(const QString& src, const QString& dest)
{
    if (! QFile::exists(src))
        return;

    if (QFile::exists(dest)) {
        if (QFile::exists(dest+".bkp"))
            QFile(dest + ".bkp").remove();

        QFile::copy(dest, dest + ".bkp");
        QFile(dest).remove();
    }

    QFile::copy(src, dest);
}

inline bool isInteger(const QString& str)
{
    bool ok = false;
    str.toInt(&ok);
    return ok;
}

inline bool isFloat(const QString& str)
{
    bool ok = false;
    str.toFloat(&ok);
    return ok;
}

inline bool isNumber(const QString& str)
{
    return isInteger(str) || isFloat(str);
}

inline bool isPercentSize(const QString& value)
{
    return value.indexOf(QRegExp("[0-9]+\%")) != -1;
}

inline int parseSize(const QString& value)
{
    if (isInteger(value))
        return value.toInt();
    if (isPercentSize(value)) {
        QStringList parts = value.split("%", QString::SkipEmptyParts);
        if (! parts.isEmpty() && isInteger(parts[0]))
            return parts[0].toInt();
    }

    return 0;
}

//Copied from: http://john.nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
inline bool removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

inline int fontSize(const QString& font)
{
    bool ok = false;
    int size = font.toInt(&ok);
    if (ok)
        return size;

    if (! font.contains("px"))
        return 0;

    QStringList fontParts = font.split("px", QString::SkipEmptyParts);
    if (fontParts.isEmpty() || ! Utils::isNumber(fontParts[0]))
        return 0;
    return fontParts[0].toInt();
}

inline QString fontFamily(const QString& font)
{
    QRegExp fontpattern("^[0-9]+px[ ]+(.+)$");
    if (fontpattern.exactMatch(font)) {
        QStringList textparts = fontpattern.capturedTexts();
        return textparts.last();
    }

    return font;
}

inline QString font(int fontSize, const QString& fontFamily) {
    return QString("%1px %2").arg(fontSize).arg(fontFamily);
}

inline QString fontFace(const QString& filename, const QString& family)
{
    QStringList lines;

    lines << "@font-face {"
          << QString("font-family: '%1';").arg(family)
          << QString("src: url('%1.eot?#iefix') format('embedded-opentype'),").arg(filename)
          << QString("url('%1.ttf') format('truetype'),").arg(filename)
          << QString("url('%1.otf') format('opentype'),").arg(filename)
          << QString("url('%1.woff') format('woff'),").arg(filename)
          << QString("url('%1.svg#%1') format('svg');").arg(filename)
          << QString("font-weight: normal;")
          << QString("font-style: normal;")
          << "}";

    return lines.join("\n");
}

}

namespace FileDialogUtils {
    extern const QString SoundFilter;
    extern const QString ImageFilter;
    extern const QString FontFilter;

    QString getOpenImageFileName(QWidget *parent = Q_NULLPTR, const QString &dir = QString());
    QString getOpenSoundFileName(QWidget *parent = Q_NULLPTR, const QString &dir = QString());
    QString getOpenFontFileName(QWidget *parent = Q_NULLPTR, const QString &dir = QString());
    QString toFilter(const QString& name, const QStringList& ext);
}

#endif // UTILS_H
