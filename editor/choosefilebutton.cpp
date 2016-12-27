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

#include "choosefilebutton.h"
#include "assetmanager.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QtDebug>


ChooseFileButton::ChooseFileButton(QWidget *parent) :
    QPushButton(parent)
{
    init(NoFilter);
}

ChooseFileButton::ChooseFileButton(FilterType filter, QWidget *parent) :
    QPushButton(parent)
{
    init(filter);
}

void ChooseFileButton::init(FilterType filter)
{
    connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
    mFilePath = "";
    mPreviousPath = "";
    mImageExtensions << "png" << "xpm" << "jpg" << "jpeg" << "gif" << "svg" << "svgz";
    mSoundExtensions << "ogg" << "oga" << "mp3" << "aac" << "wav" << "webm";
    setFilter(filter);
    mPlaceholderText = tr("No File Selected");
    setText(mPlaceholderText);
}

void ChooseFileButton::onClick()
{
    if (! mFilePath.isEmpty()) {
        QMessageBox msgBox(QMessageBox::NoIcon, tr("What do you want to do?"), "");
        QPushButton * chooseButton;
        QPushButton * removeButton;

        if (mActiveFilter == ImageFilter) {
            chooseButton = new QPushButton(tr("Choose new image"), &msgBox);
            removeButton = new QPushButton(tr("Remove current image"), &msgBox);
        }
        else {
            chooseButton = new QPushButton(tr("Choose new sound"), &msgBox);
            removeButton = new QPushButton(tr("Remove current sound"), &msgBox);
        }

        msgBox.addButton(chooseButton, QMessageBox::AcceptRole);
        msgBox.addButton(removeButton, QMessageBox::ResetRole);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(chooseButton);
        msgBox.exec();

        if (msgBox.clickedButton() == removeButton) {
            setFilePath("");
            emit fileSelected("");
        }

        if (msgBox.clickedButton() != chooseButton)
            return;
    }

    QString filePath = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                       mPreviousPath,
                                       mFilters);

    if (! filePath.isEmpty()) {
        QFileInfo info(filePath);
        mPreviousPath = info.absolutePath();
        if (mActiveFilter == ImageFilter && mImageExtensions.contains(info.suffix())) {
            emit fileSelected(filePath);
            setFilePath(filePath);
        }
        else if (mActiveFilter == SoundFilter && mSoundExtensions.contains(info.suffix())) {
            emit fileSelected(filePath);
            setFilePath(filePath);
        }
    }
}

QString ChooseFileButton::filePath()
{
    return mFilePath;
}

void ChooseFileButton::setFilePath(const QString & path)
{
    if (mFilePath == path)
        return;

    mFilePath = path;

    if (mFilePath.isEmpty()) {
        setText(mPlaceholderText);
        setIcon(QIcon());
        return;
    }

    QFileInfo file(mFilePath);
    QIcon icon;

    if (file.exists()) {
        if (mActiveFilter == ImageFilter)
            icon = QIcon(mFilePath);
    }

    setText(file.fileName());
    setIcon(icon);
}

void ChooseFileButton::setImageFile(ImageFile* image)
{
    if (image) {
        mFilePath = image->path();
        setText(QFileInfo(mFilePath).fileName());
        setIcon(QIcon(image->pixmap()));
    }
    else {
        setFilePath("");
    }
}

bool ChooseFileButton::hasValidFile()
{
    return ! mFilePath.isEmpty();
}

void ChooseFileButton::setFilter(FilterType filter)
{
    QString filters("");
    QString extensions("");

    switch(filter) {
        case ImageFilter:
        foreach(const QString ext, mImageExtensions)
            extensions += QString("*.%1 ").arg(ext);
        filters += QString("Images(%1);;").arg(extensions);
        break;
        case SoundFilter:
        foreach(const QString ext, mSoundExtensions)
            extensions += "*." + ext + " ";
        filters += QString("Sounds(%1);;").arg(extensions);
        break;
    }

    filters += "Any(*.*)";
    mFilters = filters;
    mActiveFilter = filter;
}

QString ChooseFileButton::placeholderText() const
{
    return mPlaceholderText;
}

void ChooseFileButton::setPlaceholderText(const QString & text)
{
    if (mPlaceholderText == text)
        return;

    mPlaceholderText = text;

    if (!hasValidFile())
        setText(mPlaceholderText);
}
