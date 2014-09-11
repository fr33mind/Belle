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

#ifndef ADD_OBJECT_H
#define ADD_OBJECT_H

#include <QObject>
#include <QString>

#include "ui_add_character.h"
#include "character.h"

class AddCharacterDialog : public QDialog {

    Q_OBJECT

    QString mPath;
    QString mPrevName;
    Ui::AddCharacterDialog mUi;
    QHash<QString, QPixmap> mPixmapCache;

public:
    explicit AddCharacterDialog(QWidget *parent = 0);
    AddCharacterDialog(Character*, QWidget *parent = 0);
    QString path();
    QString name();
    QHash<QString, QString> statesAndImagePaths();
    void accept();

protected:
    QString imagePath(QTreeWidgetItem*);
    void addState(const QString&, const QString&);

private:
    void init(Character* character=0);
    void clearCache(QTreeWidgetItem*);

private slots:
    void onImageSelected(const QString&);
    void onWidgetStateChanged(bool);
    void onItemClicked(QTreeWidgetItem *, int);
    void onItemChanged(QTreeWidgetItem *, int);
    void addNewState();
    void removeState();
    void onStateEdited(const QString&);

};

#endif // ADD_OBJECT_H
