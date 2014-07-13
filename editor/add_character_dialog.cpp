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

AddCharacterDialog::AddCharacterDialog(QWidget *parent) :
    QDialog(parent)
{
    mUi.setupUi( this );
    setModal(true);

    mUi.lImage->setAlignment(Qt::AlignCenter);
    mUi.statusTreeWidget->setToolTip(tr("Double click to add a new status"));
    mUi.statusTreeWidget->viewport()->installEventFilter(this);

    addNewStatus();
    connect(mUi.statusTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onItemClicked(QTreeWidgetItem *, int)));
    connect(mUi.statusTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem *, int)));

    show();
}


void AddCharacterDialog::accept()
{
    if (mUi.nameEdit->text().isEmpty()) {
        mUi.errorLabel->setText("Name is empty.");
        return;
    }

    if (mUi.statusTreeWidget->topLevelItemCount() == 0) {
        mUi.errorLabel->setText("You need to add at least one status.");
        return;
    }

    QTreeWidgetItem* item = mUi.statusTreeWidget->topLevelItem(mUi.statusTreeWidget->topLevelItemCount()-1);
    ChooseFileButton* chooseFileButton = static_cast<ChooseFileButton*>(mUi.statusTreeWidget->itemWidget(item, 1));
    if (! chooseFileButton->hasValidFile()) {
        mUi.errorLabel->setText(QString("Status '%1' doesn't have any image.").arg(item->text(0)));
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
    if (! path.isEmpty())
        onImageSelected(path);
}

QString AddCharacterDialog::imagePath(QTreeWidgetItem * item)
{
    ChooseFileButton* chooseFileButton = static_cast<ChooseFileButton*>(mUi.statusTreeWidget->itemWidget(item, 1));
    if (! chooseFileButton)
        return "";

    return chooseFileButton->filePath();
}

QIcon AddCharacterDialog::icon(QTreeWidgetItem * item)
{
    ChooseFileButton* chooseFileButton = static_cast<ChooseFileButton*>(mUi.statusTreeWidget->itemWidget(item, 1));
    if (! chooseFileButton)
        return QIcon();

    return chooseFileButton->icon();
}

QString AddCharacterDialog::path()
{
    return mPath;
}

void AddCharacterDialog::onImageSelected(const QString& path)
{
    QPixmap pixmap(path);
    if (pixmap.height() > mUi.lImage->height())
        pixmap = pixmap.scaledToHeight(mUi.lImage->height());
    mUi.lImage->setPixmap(pixmap);
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

bool AddCharacterDialog::eventFilter(QObject * obj, QEvent *ev)
{
    if (obj == mUi.statusTreeWidget->viewport() && ev->type() == QEvent::MouseButtonDblClick) {
        addNewStatus();
    }

    return false;
}

void AddCharacterDialog::addNewStatus()
{
    QTreeWidgetItem *item = 0;
    QString defaultStatus = "default";

    if (mUi.statusTreeWidget->topLevelItemCount() > 0) {
        item = mUi.statusTreeWidget->topLevelItem(mUi.statusTreeWidget->topLevelItemCount()-1);
        ChooseFileButton* chooseFileButton = static_cast<ChooseFileButton*>(mUi.statusTreeWidget->itemWidget(item, 1));
        if (! chooseFileButton)
            return;

        if (! chooseFileButton->hasValidFile())
            return;

        int counter = 0;
        for(int i=0; i < mUi.statusTreeWidget->topLevelItemCount(); i++) {
            item = mUi.statusTreeWidget->topLevelItem(i);
            if (item->text(0).contains("default"))
                counter++;
        }

        if (counter > 0)
            defaultStatus += QString::number(counter);
    }


    item = new QTreeWidgetItem(QStringList()<< defaultStatus << "" << "");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    mUi.statusTreeWidget->addTopLevelItem(item);

    QPushButton *removeBtn = new QPushButton(QIcon(":/media/delete.png"), "", mUi.statusTreeWidget);
    ChooseFileButton *chooseBtn = new ChooseFileButton(ChooseFileButton::ImageFilter, mUi.statusTreeWidget);
    mUi.statusTreeWidget->setItemWidget(item, 1, chooseBtn);
    connect(chooseBtn, SIGNAL(fileSelected(const QString&)), this, SLOT(onImageSelected(const QString&)));
    mUi.statusTreeWidget->setItemWidget(item, 2, removeBtn);
    connect(removeBtn, SIGNAL(clicked()), this, SLOT(removeStatus()));

    for(int i=0; i < mUi.statusTreeWidget->columnCount(); i++)
        mUi.statusTreeWidget->resizeColumnToContents(i);
}

void AddCharacterDialog::removeStatus()
{
    int index = -1;
    QTreeWidgetItem *item = 0;
    QObject* _sender = sender();

    for(int i=0; i < mUi.statusTreeWidget->topLevelItemCount(); i++) {
        item = mUi.statusTreeWidget->topLevelItem(i);
        if (mUi.statusTreeWidget->itemWidget(item, 2) == _sender) {
            index = i;
            break;
        }
    }

    if (index != -1)
        mUi.statusTreeWidget->takeTopLevelItem(index);
}

