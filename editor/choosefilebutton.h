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

#ifndef CHOOSEFILEBUTTON_H
#define CHOOSEFILEBUTTON_H

#include "imagefile.h"

#include <QPushButton>


class ChooseFileButton : public QPushButton
{
    Q_OBJECT

public:
    enum FilterType {
        ImageFilter,
        SoundFilter,
        NoFilter
    };

    explicit ChooseFileButton(QWidget *parent = 0);
    explicit ChooseFileButton(FilterType filter=NoFilter, QWidget *parent = 0);
    QString filePath();
    void setFilePath(const QString&);
    void setImageFile(ImageFile*);
    bool hasValidFile();
    void setFilter(FilterType);

signals:
    void fileSelected(const QString&);


public slots:
    void onClick();

private:
    QString mFilePath;
    QString mPreviousPath;
    QString mFilters;
    QStringList mImageExtensions;
    QStringList mSoundExtensions;
    FilterType mActiveFilter;
    void init(FilterType);

};

#endif // CHOOSEFILEBUTTON_H
