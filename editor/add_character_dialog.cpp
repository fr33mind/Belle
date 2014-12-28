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

#include "add_character_dialog.h"

#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QFileDialog>

#include "combobox.h"
#include "choosefilebutton.h"

AddCharacterDialog::AddCharacterDialog(Character* character, QWidget *parent) :
    QDialog(parent)
{
    init(character);

}

AddCharacterDialog::AddCharacterDialog(QWidget *parent) :
    QDialog(parent)
{
    init();
}

void AddCharacterDialog::init(Character* character)
{
    mUi.setupUi( this );
    setModal(true);

    mPrevName = "";
    mUi.lImage->setAlignment(Qt::AlignCenter);
    mUi.browseImageButton->setFilter(ChooseFileButton::ImageFilter);
    mUi.statusTreeWidget->header()->setSectionResizeMode(0,QHeaderView::Stretch);
    mUi.statusTreeWidget->setIconSize(QSize(32, 32));

    if (character) {
        this->setWindowTitle(tr("Edit Character"));
        mUi.nameEdit->setText(character->name());
        QHash<QString, ImageFile*> statesToImages = character->statesToImages();
        QHashIterator<QString, ImageFile*> it(statesToImages);
        while(it.hasNext()) {
            it.next();
            ImageFile* image = it.value();
            if (image) {
                QPixmap pixmap = image->pixmap();
                if (pixmap.height() > mUi.lImage->height())
                    pixmap = pixmap.scaledToHeight(mUi.lImage->height(), Qt::SmoothTransformation);
                mPixmapCache.insert(image->path(), pixmap);
                addState(it.key(), image->path());
            }
        }
    }

    connect(mUi.statusTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onItemClicked(QTreeWidgetItem *, int)));
    connect(mUi.statusTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem *, int)));

    connect(mUi.stateEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onStateEdited(const QString&)));
    connect(mUi.browseImageButton, SIGNAL(fileSelected(const QString&)), this, SLOT(onImageSelected(const QString&)));
    connect(mUi.addStateButton, SIGNAL(clicked()), this, SLOT(addNewState()));

    show();
}

void AddCharacterDialog::accept()
{
    if (mUi.nameEdit->text().isEmpty()) {
        mUi.errorLabel->setText("Name is empty.");
        return;
    }

    if (mUi.statusTreeWidget->topLevelItemCount() == 0) {
        mUi.errorLabel->setText(tr("You need to add at least one state."));
        return;
    }

    QDialog::accept();
}

void AddCharacterDialog::onItemChanged(QTreeWidgetItem * item, int column)
{
    QTreeWidgetItem *_item = 0;

    if (item->text(0).isEmpty()) {
        item->setText(0, mPrevName);
        return;
    }

    for(int i=0; i < mUi.statusTreeWidget->topLevelItemCount(); i++) {
        _item =  mUi.statusTreeWidget->topLevelItem(i);

       if (_item != item && _item->text(0) == item->text(0)){
           item->setText(0, mPrevName);
           break;
       }
    }

    mUi.statusTreeWidget->resizeColumnToContents(0);
}

void AddCharacterDialog::onItemClicked(QTreeWidgetItem * item, int column)
{
    if (mPrevName == item->text(0))
        return;

    mPrevName = item->text(0);

    QString path = imagePath(item);
    QPixmap pixmap = mPixmapCache.value(path, QPixmap());
    if (! pixmap.isNull())
        mUi.lImage->setPixmap(pixmap);
}

QString AddCharacterDialog::imagePath(QTreeWidgetItem * item)
{
    return item->data(0, Qt::UserRole).toString();
}

QString AddCharacterDialog::path()
{
    return mPath;
}

void AddCharacterDialog::onImageSelected(const QString& path)
{
    if (path.isEmpty()) {
        mUi.addStateButton->setDisabled(true);
        return;
    }

    QPixmap pixmap(path);
    if (pixmap.height() > mUi.lImage->height())
        pixmap = pixmap.scaledToHeight(mUi.lImage->height(), Qt::SmoothTransformation);
    mPixmapCache.insert(path, pixmap);
    if (! pixmap.isNull() && ! mUi.stateEdit->text().isEmpty())
        mUi.addStateButton->setDisabled(false);
}

void AddCharacterDialog::onWidgetStateChanged(bool valid)
{
    mUi.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}


QString AddCharacterDialog::name()
{
    return mUi.nameEdit->text();
}

QHash<QString, QString> AddCharacterDialog::statesAndImagePaths()
{
    QTreeWidgetItem* item = 0;
    QHash<QString, QString> statesToPaths;

    for(int i=0; i < mUi.statusTreeWidget->topLevelItemCount(); i++) {
        item = mUi.statusTreeWidget->topLevelItem(i);
        statesToPaths.insert(item->text(0), imagePath(item));
    }

    return statesToPaths;
}

void AddCharacterDialog::addNewState()
{
    QString state = mUi.stateEdit->text();
    QString filepath = mUi.browseImageButton->filePath();
    addState(state, filepath);
    mUi.stateEdit->setText("");
    mUi.browseImageButton->setFilePath("");
    mUi.addStateButton->setDisabled(true);
}

void AddCharacterDialog::addState(const QString& state, const QString& imagePath)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(mUi.statusTreeWidget);
    item->setText(0, state);
    item->setIcon(0, QIcon(imagePath));
    item->setData(0, Qt::UserRole, imagePath);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    QPushButton *removeBtn = new QPushButton(QIcon(":/media/delete.png"), "", mUi.statusTreeWidget);
    removeBtn->setFlat(true);
    connect(removeBtn, SIGNAL(clicked()), this, SLOT(removeState()));
    mUi.statusTreeWidget->setItemWidget(item, 1, removeBtn);
    for(int i=0; i < mUi.statusTreeWidget->columnCount(); i++)
        mUi.statusTreeWidget->resizeColumnToContents(i);
}

void AddCharacterDialog::removeState()
{
    int index = -1;
    QTreeWidgetItem *item = 0;
    QObject* _sender = sender();

    if (! _sender)
        return;

    for(int i=0; i < mUi.statusTreeWidget->topLevelItemCount(); i++) {
        item = mUi.statusTreeWidget->topLevelItem(i);
        if (mUi.statusTreeWidget->itemWidget(item, 1) == _sender) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        mUi.statusTreeWidget->takeTopLevelItem(index);
        mUi.lImage->setPixmap(QPixmap());
        clearCache(item);
        _sender->deleteLater();
    }
}

void AddCharacterDialog::clearCache(QTreeWidgetItem * item)
{
    QString path = imagePath(item);
    if (mPixmapCache.contains(path))
        mPixmapCache.remove(path);
}

void AddCharacterDialog::onStateEdited(const QString & text)
{
    if (text.isEmpty())
        mUi.addStateButton->setDisabled(true);
    else if (mUi.browseImageButton->hasValidFile())
        mUi.addStateButton->setDisabled(false);
}

